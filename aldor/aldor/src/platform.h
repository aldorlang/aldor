/*****************************************************************************
 *
 * platform.h: Determine compilation environment.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file defines the environment by specifying the operating system,
 * compiler, and hardware.
 *
 * If the macro CONFIG is defined (e.g. from the cc command line) then this
 * file assumes that the configuration variables have been set explicitly.
 * Otherwise, it attempts to determine the platform by examining the
 * preprocessor environment.
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*
 * Operating system:
 *
 *   Defines one of
 *      OS_MS_DOS, OS_IBM_OS2, OS_CMS, OS_VMS, OS_UNIX,
 *      OS_MAC_SYS7, OS_WIN32, OS_UNKNOWN
 *
 *   and, if OS_UNIX, one of
 *      OS_AIX_PS2, OS_AIX_RT, OS_AIX_RS, OS_AIX_370, OS_AIX_ESA,
 *      OS_SUN, OS_SUNOS5, OS_OSF1, OS_UNICOS, OS_LINUX, OS_IRIX, OS_NEXT, 
 *	OS_ULTRIX, OS_HPUX, OS_MAC_OSX
 *
 * Compiler:
 *
 *   Defines one of
 *      CC_BORLAND, CC_METAWARE, CC_MICROSOFT, CC_GNU,
 *	CC_SUN, CC_DEC_AXP, CC_CRAY, CC_MIPS, CC_MPW, CC_MWERKS, CC_ULTRIX,
 *      CC_XLC, CC_IBM_PS2, CC_IBM_RT, CC_C370, CC_CSET2, CC_HPUX, CC_SPARCCOMPILER
 *      CC_UNKNOWN
 *
 * Hardware:
 *
 *   Defines one of
 *      HW_8086, HW_80386, HW_68K, HW_PPC, HW_IA64
 *      HW_370, HW_RT, HW_RS,
 *      HW_SPARC, HW_SPARC_64, HW_AXP, 
 *      HW_MIPS,
 *      HW_CRAY, HW_VAX, HW_HPPA
 *      HW_UNKNOWN
 *
 *
 * Configuration:
 *
 *   Defines CONFIG to be a string describing the configuration.
 *   Defines CONFIGSYS to be a more easily parsed version,
 *	typically $MACHINE
 *   If the build environment is different to the runtime env,
 *   then it is of the form $RTENV:$MACHINE
 */

#if !defined(CONFIG)

#if defined(__MSDOS__) && (defined(__TURBOC__) || defined(__BORLANDC__))
# define        OS_MS_DOS
# define        CC_BORLAND
# define        HW_8086
# define        CONFIG  "DOS [+]"
# define	CONFIGSYS "dosborland"
#endif

#if defined(MSDOS) && defined(_MSC_VER)
# define        OS_MS_DOS
# define        CC_MICROSOFT
# define        HW_8086
# define        CONFIG  "DOS [*]"
# define	CONFIGSYS "dosmsvc"
#endif

#if defined(__MSDOS__) && defined(__GNUC__)
# define        OS_MS_DOS
# define        CC_GNU
# define        HW_80386
# define        CONFIG  "DOS [=]"
# define	CONFIGSYS "dosgcc"
#endif

#if defined(__WIN32__) && defined(__BORLANDC__)
# define        OS_WIN32
# define        CC_BORLAND
# define        HW_80386
# define        CONFIG  "Win32 [+]"
# define	CONFIGSYS "win32borland"
#endif

#if defined(_WIN32) && defined(_MSC_VER)
# define        OS_WIN32
# define        CC_MICROSOFT
# define        HW_80386
# define        CONFIG  "Win32 [+]"
# define	CONFIGSYS "win32msvc"
#endif

#if defined(__CYGWIN__) && defined(__GNUC__)
# define        OS_WIN32
# define        CC_GNU
# define        HW_80386
# define        CONFIG  "Win32 [+]"
# define	CONFIGSYS "win32cygwin"
#endif

#if defined(__WIN32__) && defined(__GNUC__)
# define        OS_WIN32
# define        CC_GNU
# define        HW_80386
# define        CONFIG  "Win32 [+]"
# define	CONFIGSYS "win32gcc"
#endif

#if defined(__NT__) && defined(__WATCOMC__)
# define        OS_WIN32
# define        CC_WATCOM
# define        HW_80386
# define        CONFIG  "Win32 [+]"
#endif

#if defined(__MWERKS__)
# define	OS_MAC_SYS7
# define	CC_MWERKS
# define	HW_PPC
# define	CONFIG "Macintosh System 7"
#endif

#if defined(macintosh) && defined(__DUMP__)
# define	OS_MAC_SYS7
# define	CC_MPW
# define	HW_68K
# define	CONFIG "Macintosh System 7"
# define	CONFIGSYS "macsys7"
#endif

/* This doesn't properly distinguish from Mac Classic version 9 */
#if defined(__APPLE__) && defined(__GNUC__)
# define    OS_UNIX
# define	OS_MAC_OSX
# define	CC_GNU
# define	CONFIG "Macintosh OS X -- darwin"
# define	CONFIGSYS "macOSX"
#if defined(__ppc__) 
# define	HW_PPC
#else /* This is an intel Macintosh */
# define HW_80386
#endif /* __ppc__ specific */
#endif /* __APPLE__ specific */

/* Someone stop this proliferation of incompatible glibcs! */
#if defined(__linux__) && defined(__GNUC__)

# define OS_UNIX
# define OS_LINUX
# define CC_GNU

# if defined(__i386__)
#    define HW_80386
# endif
# if defined(__alpha__)
#    define HW_AXP
# endif
# if defined(__ia64) || defined(__ia64__) || defined(__IA64) || defined(__IA64__)
#  define HW_IA64
# endif


     /* GCC 2.96 shipped with RH 7.0 is severly broken ... */
# if defined(__GNUC_MINOR__) && __GNUC__ == 2 && __GNUC_MINOR__ == 96
#   if defined(HW_AXP)
#     define     CONFIG "LINUX(gcc-2_96, alpha)"
#     define     CONFIGSYS "linuxglibc-2_96-alpha"
#   else
#     define     CONFIG "LINUX(gcc-2_96)"
#     define     CONFIGSYS "linuxglibc-2_96"
#   endif
# else
#  include <features.h>
#  if defined(__clang__)
#    define CONFIG "LINUX(clang)"
#    define CONFIGSYS "linuxclang"
#  elif defined(__GLIBC__) && __GLIBC__ == 2
#    if defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 0 
#      define	CONFIG "LINUX(glibc2.0)"
#      define	CONFIGSYS "linuxglibc"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 1
#      define      CONFIG "LINUX(glibc2.1)"
#      define      CONFIGSYS "linuxglibc2.1"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 2
#      define      CONFIG "LINUX(glibc2.2)"
#      define      CONFIGSYS "linuxglibc2.2"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 3
#      define      CONFIG "LINUX(glibc2.3)"
#      define      CONFIGSYS "linuxglibc2.3"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 4
#      define      CONFIG "LINUX(glibc2.4)"
#      define      CONFIGSYS "linuxglibc2.4"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 5
#      define      CONFIG "LINUX(glibc2.5)"
#      define      CONFIGSYS "linuxglibc2.5"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ == 6
#      define      CONFIG "LINUX(glibc2.6)"
#      define      CONFIGSYS "linuxglibc2.6"
#    elif defined (__GLIBC_MINOR__) && __GLIBC_MINOR__ >= 10
#      define      CONFIG "LINUX(glibc2.10+)"
#      define      CONFIGSYS "linuxglibc2.10+"
#    else
#      define      CONFIG "LINUX(glibc-unknown)" 
#      define      CONFIGSYS "linuxglibc-unknown"
#    endif
#  elif defined(__GLIBC__)
#    define     CONFIG "LINUX(glibc-unknown)"
#    define     CONFIGSYS "linuxglibc-unknown"
#  else
#    define	CONFIG "LINUX"
#    define	CONFIGSYS "linux"
#  endif
# endif
#endif

#if defined(__OS2__) && defined(__BORLANDC__)
# define        OS_IBM_OS2
# define        CC_BORLAND
# define        HW_80386
# define        CONFIG  "OS/2 [+]"
# define	CONFIGSYS "os2borland"
#endif

#if defined(__OS2__) && (defined(__IBMC__) || defined(__IBMCPP__))
# define        OS_IBM_OS2
# define        CC_CSET2
# define        HW_80386
# define        CONFIG  "OS/2 [*]"
# define	CONFIGSYS "os2borland"
#endif

#if defined(vax) && defined(ultrix)
#define OS_ULTRIX
#define OS_UNIX
#define CC_ULTRIX
#define  HW_VAX
#define CONFIG "Vax (Ultrix 4.2 ish)"
# define	CONFIGSYS "vms"
#endif

/* 
 * There are 5 Sparc possibilities:
 *  1) solaris: Compile with gcc, for gcc
 *  2) solaris: Compile with cc,  for gcc
 *  3) solaris: Compile with cc,  for cc
 *  4) sunos4:  Compile with cc,  for cc (antique c) (is deprecated)
 *  5) sunos4:  Compile with gcc, for gcc
 */

#if defined(__sparc__) && defined(__GNUC__) && defined(__svr4__)
# define OS_UNIX
# define OS_SUN
# define OS_SUNOS5
# define CC_GNU
# if defined(__arch64__)
#  define CONFIG     "SPARC [Solaris: GCC 64-bit]"
#  define HW_SPARC_64
#  define CONFIGSYS "sun4os55g64"
# else
#  define CONFIG     "SPARC [Solaris: GCC]"
#  define HW_SPARC
#  define CONFIGSYS "sun4os55g"
# endif
#endif

#if defined(__sparc) && defined(__SVR4) && defined(__USE_GCC) && defined(__SUNPRO_C) && !defined(CONFIG)
# define OS_UNIX
# define OS_SUN
# define OS_SUNOS5
# define CC_SPARCCOMPILER
# define HW_SPARC
# define CONFIG "SPARC [Solaris: SPARCCompiler front, GCC back]"
# define CONFIGSYS "sun4os55g:sun4os55c"
#endif


#if defined(__sparc) && defined(__SVR4) && defined(__USE_CC) && defined(__SUNPRO_C) && !defined(CONFIG)
# define OS_UNIX
# define OS_SUN
# define OS_SUNOS5
# define CC_SPARCCOMPILER
# define HW_SPARC
# define CONFIG "SPARC [Solaris: SPARCCompiler]"
# define CONFIGSYS "sun4os55c"
#endif

#if defined(sparc) && !defined(__GNUC__) && !defined(CONFIG)
# define        OS_UNIX
# define        OS_SUN
# define        OS_SUNOS4
# define        CC_SUN
# define        HW_SPARC
# define        CONFIG  "SPARC (CC)"
# define CONFIGSYS "sun4os4c"
#endif


#if defined(sparc) && defined(__GNUC__) && !defined(CONFIG)
# define        OS_UNIX
# define        OS_SUN
# define        OS_SUNOS4
# define        CC_GNU
# define        HW_SPARC
# define        CONFIG  "SPARC [GCC]"
# define CONFIGSYS "sun4os4g"
#endif

#if defined(sparc) && !defined(CONFIG)
# define        OS_UNIX
# define        OS_SUN
# define        OS_SUNOS4
# define        CC_UNKNOWN
# define        HW_SPARC
# define        CONFIG  "SPARC [*]"
# define CONFIGSYS "sun4os4c"
#endif

#if defined(i386) && defined(aiws)
# define        OS_UNIX
# define        OS_AIX_PS2
# define        CC_IBM_PS2
# define        HW_80386
# define        CONFIG  "AIX PS/2"
#endif

#if !defined(i386) && defined(aiws)
# define        OS_UNIX
# define        OS_AIX_RT
# define        CC_IBM_RT
# define        HW_RT
# define        CONFIG  "AIX RT"
# define CONFIGSYS "aixrt"
#endif

#if defined(_IBMR2) && !defined(__GNUC__)
/* 
 we will ignore the _POWER that may be defined on PowerPC platforms 
*/
# define        OS_UNIX
# define        OS_AIX_RS
#if defined _AIX41
# define        OS_AIX41_RS
# define CONFIGSYS "rs4"
#elif defined _AIX32
# define        OS_AIX32_RS
# define CONFIGSYS "rs3.2"
#endif
# define        CC_XLC
# define        HW_RS
# define        CONFIG  "AIX RS/6000"
#endif

#if defined(_IBMR2) && defined(__GNUC__)
# define        OS_UNIX
# define        OS_AIX_RS
# define        CC_GNU
# define        HW_RS
# define        CONFIG  "AIX RS/6000 [=]"
# define CONFIGSYS "rsgcc"
#endif

#if defined(_IBMR2) && !defined(CONFIG)
# define        OS_UNIX
# define        OS_AIX_RS
# define        CC_UNKNOWN
# define        HW_RS
# define        CONFIG  "AIX RS/6000 [+]"
# define CONFIGSYS "rs3.2"
#endif

#if defined(__alpha) && defined(__osf__)
# define	OS_UNIX
# define	OS_OSF1
# define	CC_DEC_AXP
# define	HW_AXP
# define	CONFIG  "OSF/1 AXP"
# if defined(__DECC_VER)
#  define 	CONFIGSYS "axposf1v4"
#  else
#  define        CONFIGSYS "axposf1v3"
# endif
#endif

#if defined(__ALPHA) && defined(__VMS) && defined(__DECC)
# define	OS_VMS
# define	CC_DEC_AXP
# define	HW_AXP
# define	CONFIG  "OpenVMS/AXP"
# define 	CONFIGSYS "axpvms"
#endif

#if defined(__sgi) && defined(__mips) && !defined(__GNUC__) 
# define        OS_UNIX
# define        OS_IRIX
# define        CC_MIPS
#if defined(_MIPS_ISA) 
#if  __mips == 1 
#       define         HW_MIPS 1
#       define CONFIG "IRIX MIPS1"
#       define CONFIGSYS "irixmips1"
#elif __mips == 2
#       define         HW_MIPS 2
#       define CONFIG "IRIX MIPS2"
# 	define CONFIGSYS "irixmips2"
#elif __mips == 3
#       define         HW_MIPS 3
#       define CONFIG "IRIX MIPS3"
# 	define CONFIGSYS "irixmips3"
#elif __mips == 4
#       define         HW_MIPS 4
#       define CONFIG "IRIX MIPS4"
# 	define CONFIGSYS "irixmips4"
#endif 
#endif 
#endif

#if defined(__sgi) && defined(__mips) && defined(__GNUC__)
# define        OS_UNIX
# define        OS_IRIX
# define        CC_GNU
# define        HW_MIPS
# define        CONFIG "IRIX GNU"
# define        CONFIGSYS "irixgcc"

#endif

#if defined(__hppa) && defined(__hpux)
# define OS_UNIX
# define OS_HPUX
# define CC_HPUX
# define HW_HPPA
# define CONFIG "HPPA HPUX"
# define CONFIGSYS "hpux"
#endif

#if defined(__NeXT__) && defined(__m68k__) && defined(__GNUC__)
# define        OS_UNIX
# define        OS_NEXT
# define        CC_GNU
# define        HW_68K
# define        CONFIG  "NeXT M680x0"
# define 	CONFIGSYS "nextm68k"
#endif

#if defined(CRAY)
# define	OS_UNIX
# define	OS_UNICOS
# define	CC_CRAY
# define	HW_CRAY
# define	CONFIG  "CRAY UNICOS YMP"
# define 	CONFIGSYS "crayymp"
#endif

#if defined(_AIX370)
# define        OS_UNIX
# define        OS_AIX_370
# define        CC_METAWARE
# define        HW_370
# define        CONFIG  "AIX 370"
# define 	CONFIGSYS "aix370"
#endif

#if defined(_IBMESA)
# define        OS_UNIX
# define        OS_AIX_ESA
# define        CC_METAWARE
# define        HW_370
# define        CONFIG  "AIX ESA"
# define 	CONFIGSYS "aixesa"
#endif

#if defined(__EXTENDED__) && !defined(_IBMR2) && !defined(OS_IBM_OS2)
# define        OS_CMS
# define        CC_C370
# define        HW_370
# define        CONFIG  "CMS"
# define 	CONFIGSYS "cms"
#endif

#if !defined(CONFIG)
# define        OS_UNKNOWN
# define        CC_UNKNOWN
# define        HW_UNKNOWN
# define        CONFIG  "generic platform"
# define 	CONFIGSYS "default"
#endif

#if defined(BOMB)
# define OS_Has_License         BOMB
#endif

#endif /* !CONFIG */

#endif /* !_PLATFORM_H_ */
