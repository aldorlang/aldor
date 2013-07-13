/**
 * Copyright © 2007 Pippijn van Steenhoven
 * License: GNU Affero General Public License version 3 or higher.
 */

#include <iostream>

#include <fstream>
#include <vector>
#include <cstdarg>
#include <stdexcept>

#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <linux/ptrace.h>
#include <linux/securebits.h>
#include <unistd.h>
#include <syscall.h>
#include <poll.h>

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

#include "utf.h"
#include "syscalls.h"

// C++ doesn't like implicit conversions from int to enum
#define ptrace(req, ...)        ptrace ((__ptrace_request)req, __VA_ARGS__)

static const int kibi = 1024;
static const int mebi = 1024 * kibi;
static const int gibi = 1024 * mebi;

struct callaction
{
  const char * const name;
  syscall::action const access;

  operator bool () { return name; }
} allowed_calls[] = {
  // ignored_syscalls
  { "munmap",                   syscall::ignore },
  { "rt_sigaction",             syscall::ignore },
  { "ioctl",                    syscall::ignore },

  // allowed syscalls
  { "open",                     syscall::allow },
  { "write",                    syscall::allow },
  { "brk",                      syscall::allow },
  { "read",                     syscall::allow },
  { "mmap",                     syscall::allow },
  { "exit_group",               syscall::allow },
  { "getpid",                   syscall::allow },
  { "access",                   syscall::allow },
  { "close",                    syscall::allow },
  { "mprotect",                 syscall::allow },

  { "stat",                     syscall::allow },
  { "fstat",                    syscall::allow },
  { "arch_prctl",               syscall::allow },
  { "lseek",                    syscall::allow },
  { "times",                    syscall::allow },
  
  { 0, syscall::deny }
};

static bool
setactions ()
{
  const char *call;
  const char *nest;
  syscall::action action;

  for (int i = 0; (call = allowed_calls[i].name); i++)
    for (int j = 0; (nest = calls[j].name); j++)
      if (!strcmp (call, nest))
        {
          calls[j].access = allowed_calls[i].access;
        }

  return true;
}

static bool actions_set = setactions ();

struct auto_fd
{
  typedef int fd_type;

  explicit auto_fd (fd_type fd = -1) throw ()
  : fd_ (fd), do_throw (false)
  {
  }
  
  auto_fd (auto_fd &a) throw ()
  : fd_ (a.release ()), do_throw (a.throws ())
  {
  }
  
  auto_fd &operator = (auto_fd &a) throw ()
  {
    reset (a.release ());
    return *this;
  }

  ~auto_fd () throw ()
  {
    if (fd_ != -1)
      {
        if (do_throw)
          {
            char msg[] = "Closing unclosed fd";
            char buf[sizeof (msg) + 5];
            snprintf (buf, sizeof (buf) - 1, "%s (%d)", msg, fd_);
            throw std::runtime_error (buf);
          }
        ::close (fd_);
      }
  }
  
  fd_type get () const throw ()
  {
    return fd_;
  }
  
  fd_type release () throw ()
  {
    fd_type tmp = fd_;
    fd_ = -1;
    return tmp;
  }
  
  void reset (fd_type fd = -1) throw ()
  {
    if (fd != fd_)
      {
        ::close (fd_);
        fd_ = fd;
      }
  }

  bool throws () const throw ()
  {
    return do_throw;
  }

  int close () throw ()
  {
    if (fd_ == -1)
      throw std::runtime_error ("Attempting to close an already closed fd");
    return ::close (release ());
  }

  bool operator == (fd_type rhs) const
  {
    return fd_ == rhs;
  }

  bool operator != (fd_type rhs) const
  {
    return !(*this == rhs);
  }

private:
  fd_type fd_;
  bool do_throw;
};

template<STRLEN N>
static inline const char *
getHVvalue (HV * const hv, const char (&key)[N], const char * const dflt = "")
{
  if (!hv)
    return dflt;

  STRLEN n = N;
  SV **sv = hv_fetch (hv, key, n - 1, 0);

  return sv ? SvPVutf8 (*sv, n) : dflt;
}

// Create an SV * from string literals or character arrays
template<int N>
static inline SV *
newSVpvN (const char (&src)[N])
{
  return newSVpvn (src, N - 1);
}

static SV *
newSVpvv (const char *fmt, ...)
{
  va_list ap;
  char buf[30000];

  va_start (ap, fmt);
  int len = vsnprintf (buf, 29999, fmt, ap);
  va_end (ap);

  return newSVpvn (buf, len);
}

#ifdef HAVE_VALGRIND
#include <valgrind/memcheck.h>
static inline void
memcheck ()
{
#ifdef __x86_64__
  {
    unsigned int _qzz_res;
    {
      volatile unsigned long _zzq_args[6];
      volatile unsigned long _zzq_result;
      _zzq_args[0] = (unsigned long) (VG_USERREQ__DO_LEAK_CHECK);
      _zzq_args[1] = (unsigned long) (0);
      _zzq_args[2] = (unsigned long) (0);
      _zzq_args[3] = (unsigned long) (0);
      _zzq_args[4] = (unsigned long) (0);
      _zzq_args[5] = (unsigned long) (0);
      asm volatile ("rolq $3,  %%rdi ; rolq $13, %%rdi\n\t"
                    "rolq $61, %%rdi ; rolq $51, %%rdi\n\t"
                    "xchgq %%rbx, %%rbx"
                    : "=d" (_zzq_result)
                    : "a" (&_zzq_args[0]), "0" (0)
                    : "cc", "memory" );
      _qzz_res = _zzq_result;
    }
  }
#else
  VALGRIND_DO_LEAK_CHECK;
#endif
}
#else
static inline void
memcheck ()
{
}
#endif // HAVE_VALGRIND

namespace cpu
{
  enum reg
  {
#ifdef __x86_64__
    orig_ax  = ORIG_RAX * 8,
    ax       = RAX      * 8,
    bx       = RBX      * 8,
    cx       = RCX      * 8,
    dx       = RDX      * 8
#elif defined (__i386__)
    orig_ax  = ORIG_EAX * 4,
    ax       = EAX      * 4,
    bx       = EBX      * 4,
    cx       = ECX      * 4,
    dx       = EDX      * 4
#else
#error "This software does not support your platform"
    orig_ax,
    ax,
    bx,
    cx,
    dx
#endif
  };
}

#ifdef __GNUC__
#include <cxxabi.h>

static char *
demangle (const char * const name)
{
  int st;
  char *p = abi::__cxa_demangle (name, 0, 0, &st);
  
  switch (st)
    {
    case 0:
      return p;
    case -1:
      throw std::runtime_error ("demangle: A memory allocation failure occurred.");
    case -2:
      throw std::runtime_error ("demangle: Not a valid name under the GCC C++ ABI mangling rules.");
    case -3:
      throw std::runtime_error ("demangle: One of the arguments is invalid.");
    default:
      throw std::runtime_error ("demangle: Unexpected demangle status.");
    }
}
#else
static const char *
demangle (const char * const name)
{
  return "No demangling routines known for this compiler. Please report";
}
#endif

template<typename T>
static const char *
type_desc (T v)
{
  return demangle (typeid (T).name ());
}

struct resources
{
  resources (unsigned int max_time, rlim_t max_open_files, rlim_t max_memory, rlim_t max_filesize)
  : time (max_time)
  {
    open_files.rlim_max = max_open_files;
    open_files.rlim_cur = max_open_files;
    memory.rlim_max     = max_memory * mebi;
    memory.rlim_cur     = max_memory * mebi;
    filesize.rlim_max   = max_filesize * mebi;
    filesize.rlim_cur   = max_filesize * mebi;
  }

  resources (resources const &other)
  : time (other.time)
  {
    open_files.rlim_max = other.open_files.rlim_max;
    open_files.rlim_cur = other.open_files.rlim_cur;
    memory.rlim_max     = other.memory.rlim_max;
    memory.rlim_cur     = other.memory.rlim_cur;
    filesize.rlim_max   = other.filesize.rlim_max;
    filesize.rlim_cur   = other.filesize.rlim_cur;
  }

  void enforce () const
  {
    alarm (time);
    setrlimit (RLIMIT_FSIZE,  &filesize);
    setrlimit (RLIMIT_AS,     &memory);
    setrlimit (RLIMIT_NOFILE, &open_files);
  }

private:
  unsigned int time;
  rlimit open_files;
  rlimit memory;
  rlimit filesize;
};

static int
checked (const char * const name, const int ret, const char * const location)
{
  if (ret == -1)
//    throw std::runtime_error ("%s: Error code %d (%s)", name, errno, strerror (errno));
    throw std::runtime_error (strerror (errno) + std::string (" from ") + location);

  return ret;
}

#define _STROF(x)       #x
#define STROF(x)        _STROF (x)

#define checked(name, ret)      checked (name, ret, __FILE__ " (" STROF (__LINE__) ")")

struct executable
{
  const char * const            name;
  char * const *                argv;
  const int                     argc;
  resources const               limits;

  template<size_t argc>
  executable (char * const (&argv)[argc], resources const &limits)
  : name (argv[0]), argv (argv), argc (argc), limits (limits)
  {
  }
  
  SV *execute (char const *input = 0, bool traced = true, bool block = false) const;
};

// time(s), open_files(fds), memory(MiB), filesize(MiB)
static resources const limits ( 5, 25, 200, 10);

// compiler
static char * const compiler_args[] = { "/bin/aldor", "-fao=t.ao", "t.as", NULL };
static executable const compiler (compiler_args, limits);

// REPL
static char * const program_args[] = { "/bin/aldor", "-gloop", NULL };
static executable const program (program_args, limits);

template<int N>
static void
filter_output (char (&buf)[N], char * &bufptr)
{
  char utf8code[N] = { 0 };
  uint32_t utf32print[N];
  uint32_t *utf32printptr = utf32print;

  std::vector<uint32_t> utf32vec;
  utf::conv8_32 (buf, bufptr, std::back_inserter (utf32vec));

  std::vector<uint32_t>::const_iterator it = utf32vec.begin (),
                                            et = utf32vec.end ();
  for (; it != et; ++it)
    if (*it & ~0x1f && *it != 0x7f || *it == '\n')
      *utf32printptr++ = *it;

  utf::conv32_8 (utf32print, utf32printptr, utf8code);
  bufptr = buf;
  while (char c = utf8code[bufptr - buf])
    *bufptr++ = c;
  *bufptr = 0;
}

static SV *
getoutput (auto_fd fd)
{
  int chunksize = 10;
  ssize_t bytes = chunksize;

  char buf[30000];
  char *bufptr = buf;

  pollfd fds[] = { { fd.get (), POLLIN, 0 } };
  
  while (poll (fds, 1, 100) && bytes == chunksize && bufptr - buf < 30000)
    {
      bytes = read (fd.get (), bufptr, chunksize);
      bufptr += bytes;
    }

  if (bufptr == buf)
    {
      checked ("close", fd.close ());
      return 0;
    }

  checked ("close", fd.close ());

  char *valid_until = utf::valid_until (buf, bufptr);
  if (bufptr - valid_until)
    {
      filter_output (buf, valid_until);
      return newSVpvv ("Invalid utf8 character after: %s", buf);
    }
  
  filter_output (buf, bufptr);
  return newSVpvn (buf, bufptr - buf <= 30000 ? bufptr - buf : 30000);
}

SV *
executable::execute (char const *input, bool traced, bool block) const
{
  bool entering = false;
  int status;
  auto_fd::fd_type pipes[2];
  pid_t child;

  if (pipe (pipes) < 0)
    return 0;

#define READ    0
#define WRITE   1

  auto_fd readpipe (pipes[READ]);
  auto_fd writepipe (pipes[WRITE]);

#undef READ
#undef WRITE

  if ((child = fork ()) == -1)
    {
      checked ("close", readpipe.close ());
      checked ("close", writepipe.close ());
      return 0;
    }
  
  if (child == 0)
    {
      checked ("close", close (STDIN_FILENO));

      if (writepipe != STDOUT_FILENO)
        dup2 (writepipe.get (), STDOUT_FILENO);
      dup2 (STDOUT_FILENO, STDERR_FILENO);
      dup2 (readpipe.get (), STDIN_FILENO);

      checked ("close", readpipe.close ());
      checked ("close", writepipe.close ());
      
      limits.enforce ();
      checked ("ptrace", ptrace (PTRACE_TRACEME, 0, NULL, NULL));
      
      checked ("execvp", execvp (name, argv));
    }
  else
    {
      if (input)
        checked ("write", write (writepipe.get (), input, strlen (input)));
      checked ("close", writepipe.close ());
      wait (&status);
      if (!(WIFSTOPPED (status) && WSTOPSIG (status) == SIGTRAP))
        {
          checked ("ptrace", ptrace (PTRACE_SYSCALL, child, NULL, NULL));
          wait (&status);
          checked ("close", readpipe.close ());

          throw std::runtime_error ("executable::execute: Signal raised was not SIGTRAP or process is not stopped");
        }
      
      checked ("ptrace", ptrace (PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD));

      do
        {
          checked ("ptrace", ptrace (PTRACE_SYSCALL, child, NULL, NULL));
          wait (&status);
          
          if (WSTOPSIG (status) == SIGALRM)
            {
              checked ("ptrace", ptrace (PTRACE_KILL, child));
              wait (0);
              checked ("close", readpipe.close ());
              return newSVpvv ("Timeout: %s", name);
            }
          if (traced)
            {
              if (WSTOPSIG (status) == (SIGTRAP | 0x80))
                {
                  entering = !entering;
                  long callnr = ptrace (PTRACE_PEEKUSER, child, cpu::orig_ax, NULL);
                  if (callnr > callcnt)
                    {
                      checked ("ptrace", ptrace (PTRACE_POKEUSER, child, cpu::orig_ax, SYS_exit_group));
                      checked ("ptrace", ptrace (PTRACE_KILL, child));
                      wait (0);
                      checked ("close", readpipe.close ());
                      return newSVpvv ("Attempt to call unknown syscall (%d)", callnr);
                    }
                  if (calls[callnr].access == syscall::allow)
                    continue;
                  else if (calls[callnr].access == syscall::ignore)
                    if (entering)
                      checked ("ptrace", ptrace (PTRACE_POKEUSER, child, cpu::orig_ax, SYS_getpid));
                    else
                      checked ("ptrace", ptrace (PTRACE_POKEUSER, child, cpu::ax, 0));
                  else
                    {
                      checked ("ptrace", ptrace (PTRACE_POKEUSER, child, cpu::orig_ax, SYS_exit_group));
                      checked ("ptrace", ptrace (PTRACE_KILL, child));
                      wait (0);
                      if (callnr == SYS_tgkill) // exception thrown
                        return getoutput (readpipe);
                      checked ("close", readpipe.close ());
                      return newSVpvv ("Disallowed syscall %d (%s) called", callnr, calls[callnr].name);
                    }
                }
              else if (WSTOPSIG (status) != 0)
                {
                  kill (child, SIGKILL);
                  wait (0);
                  break;
                }
            }
        }
      while (!WIFEXITED (status));
    }

  if (WIFEXITED (status) && WEXITSTATUS (status) != 0 && traced && block)
    {
      checked ("close", readpipe.close ());
      return newSVpvv ("Subprocess %s exited with value %d", name, WEXITSTATUS (status));
    }

  if (WIFSIGNALED (status) && WIFSTOPPED (status) && traced)
    {
      checked ("close", readpipe.close ());
      return newSVpvv ("Subprocess %s died with signal %d (%s)", name, WSTOPSIG (status), strsignal (WSTOPSIG (status)));
    }

  if (WSTOPSIG (status) != 0 && traced && block)
    {
      checked ("close", readpipe.close ());
      return newSVpvv ("Subprocess %s died on signal %d (%s)", name, WSTOPSIG (status), strsignal (WSTOPSIG (status)));
    }

  return getoutput (readpipe);
}

struct Evaluator
{
  static MGVTBL vtbl;
  // Our perl object
  HV *self;

  Evaluator (const char * const _root, uid_t _uid, gid_t _gid)
  : self (0), root (_root), uid (_uid), gid (_uid), initialised (0)
  {
  }

  void setup ()
  {
    if (initialised)
      croak ("Evaluator::setup (): Attempted to initialise the evaluator twice");

    checked ("prctl", prctl (PR_SET_SECUREBITS, SECBIT_KEEP_CAPS_LOCKED | SECBIT_KEEP_CAPS));

    checked ("chroot", chroot (root));
    checked ("chdir", chdir ("/"));
    checked ("setgid", setgid (gid));
    checked ("setuid", setuid (uid));

    initialised = true;
  }

  bool inited () const
  {
    return initialised;
  }

  SV *eval (char const *code, char const *line);

private:
  // Can't copy Evaluator objects
  Evaluator (Evaluator const &other)
  : self (0), root (0), uid (0), gid (0), initialised (0)
  {
  }

private:
  const char * const root;
  uid_t const uid;
  gid_t const gid;

  bool initialised;
};

SV *
Evaluator::eval (char const *code, char const *line)
try
{
  FILE *fh = fopen ("t.as", "w");
  if (!fh)
    return newSVpvN ("could not open file: t.as");

  fputs (code, fh);
  fclose (fh);

  SV *RETVAL = compiler.execute ();
  if (RETVAL)
    return RETVAL;

  return program.execute (line);
}
catch (std::runtime_error &e)
{
  croak ("Runtime error: %s", e.what ());
}

// We just want one single C++ object
static Evaluator *eval = 0;

MGVTBL Evaluator::vtbl;

MODULE = Evaluator        PACKAGE = Evaluator

void
import (const char *klass, SV *options = 0)
    CODE:
{
    HV *hv = (HV *) SvRV (options);
    // increment refcount because otherwise we lose the hash and its contents
    // this would be a leak, but it only happens once on boot, so it's not
    // too bad
    SvREFCNT_inc ((SV *) hv);

    const char * const username = getHVvalue (hv, "username", "nobody");
    const char * const root     = getHVvalue (hv, "chroot",   "rt");
    passwd *user = getpwnam (username);
    eval = new Evaluator (root, user->pw_uid, user->pw_gid);
}

SV *
new (const char *klass)
    CODE:
{
    if (!eval->inited ())
      {
        warn ("Evaluator::new (): The evaluator was not initialised; initialising now");
        eval->setup ();
      }
    if (!eval->self)
      {
        eval->self = newHV ();
        sv_magicext (reinterpret_cast<SV *> (eval->self), 0, PERL_MAGIC_ext, &Evaluator::vtbl, reinterpret_cast<char *> (eval), 0);

        RETVAL = sv_bless (newRV_inc (reinterpret_cast<SV *> (eval->self)), gv_stashpv (klass, 1));
      }
    else
      {
        SV *sv = newRV_inc (reinterpret_cast<SV *> (eval->self));

        if (Gv_AMG (gv_stashpv (klass, 1)))
          SvAMAGIC_on (sv);

        RETVAL = sv;
      }
}
    OUTPUT:
    RETVAL

void
Evaluator::DESTROY ()
    CODE:
    croak ("Evaluator::DESTROY (): Illegally attempted to destroy the C++ evaluator");

SV *
Evaluator::eval (char *code, char *line)

void
setup ()
    CODE:
{
    if (!eval)
      {
        warn ("Evaluator::setup (): BUG: The evaluator was not instantiated yet");
        XSRETURN_UNDEF;
      }
    eval->setup ();
}

int
bits ()
    CODE:
{
#ifdef __i386__
    RETVAL = 32;
#elif defined (__x86_64__)
    RETVAL = 64;
#endif
}
    OUTPUT:
    RETVAL
