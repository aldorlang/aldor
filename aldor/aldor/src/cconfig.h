/*****************************************************************************
 *
 * cconfig.h: Configure properties of C compiler.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file gives the properties of the C compiler, allowing the rest
 * of the source to be free of #ifdefs for specific compilers.
 *
 * Note that some compilers define __STDC__ even though they come nowhere
 * close to implementing any standard, while others which are meticulous
 * do not define __STDC__ unless forced to.
 *
 * CC_short_pointer
 *	if	  sizeof(char *) == sizeof(short)
 *	otherwise sizeof(char *) == sizeof(long).
 *
 * CC_second_largest_is_int
 *      if        sizeof(long) > sizeof(int) 
 *      otherwise sizeof(long) = sizeof(int) > sizeof(short)
 *      (used in bigint)
 *
 * CC_no_void_pointer
 *	if void * is not allowed.
 *
 * CC_noncanonical_pointer
 *	if pointers to the same addres might not be ==.
 *	This is sometimes the case with compilers for segmented architectures.
 *
 * CC_no_const
 *	if the "const" modifier is not (fully) supported
 *
 * CC_no_enum_typedef
 *	if the values of enum foo must be given before it is typedefed.
 *
 * CC_no_empty_file
 *	if the compiler must have at least one definition in a file.
 *
 * CC_no_constant_questions
 *	does not consider ?: of constants to be constant
 *
 * CC_no_redefine
 *	if a second, identical #define gives a warning.
 *
 * CC_long_not_int32
 *      Set if sizeof(long) != 4 -- used to ensure that FiSInt is 32 bits
 *
 * CC_SF_is_double
 *      Set if FiSFloat is represented by the C type double
 *
 * CC_no_assert_h
 * CC_no_ctype_h
 * CC_no_errno_h
 * CC_no_float_h
 * CC_no_limits_h
 * CC_no_locale_h
 * CC_no_math_h
 * CC_no_setjmp_h
 * CC_no_signal_h
 * CC_no_stdarg_h
 * CC_no_stddef_h
 * CC_no_stdio_h
 * CC_no_stdlib_h
 * CC_no_string_h
 * CC_no_time_h
 *	if the ANSI header <xxxx.h> is not provided
 *
 * CC_missing_remove
 *	remove operation is not implemented
 *
 * CC_missing_memmove
 *	memmove operation is not implemented
 *
 * CC_missing_strcasecmp
 *	strcasecmp (etc) are not implemented, but have stricmp (etc).
 *
 * CC_nlchar_unused_static
 *	<string.h> and <ctype.h> introduce static variable, which are used
 *	only if certain "national character" are used.
 *
 * CC_undecl_flsbuf
 *	<stdio.h> uses but does not give prototypes for _flsbuf and _filbuf
 *
 * CC_broken_toupper
 *	toupper and tolower in <ctype.h> do not work properly on
 *	non-alphabetic characters
 *
 * CC_noisy_ctype
 *	isspace and friends complain about char parameters, e.g. isspace(*s).
 *
 * CC _noncc_sprintf
 *	sprintf returns the string, rather than a character count.
 *
 * CC_noncc_fprintf
 *      fprintf or printf returns something other than a character count.
 *
 * CC_sys_types_conflict
 *	cannot include <sys/types> after <stddef.h> or <stdlib.h>
 *
 * CC_sys_stdtypes_conflict
 *	cannot include <sys/stdtypes> after <stddef.h> or <stdlib.h>
 *
 * CC_stdlib_no_exits
 *	<stdlib.h> does not define EXIT_SUCCESS or EXIT_FAILURE
 *
 * CC_broken_ieeetrap
 *	<ieeetrap.h> cannot parse as an ANSI file.
 *
 * CC_math_wants_posix
 *      <math.h> threatens to fill the name space with tons of junk.
 *
 * CC_ebcdic_chars
 *	used to conditionalize on the native character set of the machine.
 *
 * CC_non_ieee_floats
 *	does not use IEEE floating point format.
 *
 * CC_little_endian
 *	the address of a single, double, int, long, etc
 *      is of the least significant byte.
 *
 * CC_small_dbl_max
 *	C compiler cannot compile the true DBL_MAX value.
 * CC_chdir_has_proto
 *  	Compiler barfs on our chdir prototype
 * CC_execlp_has_proto
 *  	Compiler barfs on our execlp prototype
 * CC_getcwd_has_proto
 *  	Compiler barfs on our getcwd prototype
 */

#ifndef _CCONFIG_H_
#define _CCONFIG_H_

#include "platform.h"

/*
 * :: PC platforms
 */
#if defined(CC_BORLAND) && defined(OS_MS_DOS)
#  define CC_CONFIGURED
#  if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#    define CC_short_pointer
#    else
#    define CC_noncanonical_pointer
#    endif
#  define CC_little_endian
#  endif

#if defined(CC_MICROSOFT) && defined(OS_MS_DOS)
#  define CC_CONFIGURED
#  define CC_noncanonical_pointer
#  define CC_little_endian
#  endif

#if defined(CC_GNU) && defined(OS_MS_DOS)
#  define CC_CONFIGURED
#  define CC_no_locale_h
#  define CC_stdlib_no_exits
#  define CC_small_dbl_max
#  define CC_little_endian
#  endif

#if defined(CC_CSET2) && defined(OS_IBM_OS2)
#  define CC_CONFIGURED
#  define CC_little_endian
#  endif

#if defined(CC_BORLAND) && defined(OS_IBM_OS2)
#  define CC_CONFIGURED
#  define CC_little_endian
#  endif

#if defined(OS_WIN32)
#  define CC_CONFIGURED
#  define CC_little_endian
#  define CC_one_byte_aligned_pointers
#  define CC_missing_strcasecmp
#  endif

#if defined(CC_IBM_PS2) && defined(OS_AIX_PS2)
#  define CC_CONFIGURED
#  define CC_no_const		/* const is buggy here */
#  define CC_no_enum_typedef
#  define CC_no_stdarg_h	/* va_start is defined with wrong no. args */
#  define CC_stdlib_no_exits
#  define CC_missing_memmove
#  define CC_little_endian
#  endif

#if defined(CC_GNU) && defined(OS_LINUX)
#  if defined(__GLIBC__) && __GLIBC__ >= 2
#    define CC_CONFIGURED
#    define CC_no_redefine
#    define CC_broken_toupper
#    define CC_noncc_sprintf
#    define CC_stdlib_no_exits
#    define CC_little_endian
#    define CC_getcwd_has_proto
#    if defined(HW_IA64)
#       define CC_second_largest_is_int
#    endif
#  else
#    define CC_CONFIGURED
#    define CC_no_redefine
#    define CC_missing_remove
#    define CC_missing_memmove
#    define CC_broken_toupper
#    define CC_noncc_sprintf
#    define CC_stdlib_no_exits
#    define CC_little_endian
#    define CC_getcwd_has_proto
#  endif
#endif

/*
 * :: MAC platforms
 */
#if defined(OS_MAC_SYS7)
#  define CC_CONFIGURED
#  endif

#if defined(OS_MAC_OSX)
#  define CC_no_empty_file
#  define CC_CONFIGURED
#  endif

/*
 * :: RT platforms
 */
#if defined(CC_IBM_RT) && defined(OS_AIX_RT)
#  define CC_CONFIGURED
#  define CC_no_void_pointer
#  define CC_no_const
#  define CC_no_constant_questions
#  define CC_missing_memmove
#  define CC_missing_remove
#  define CC_no_float_h
#  define CC_no_limits_h
#  define CC_no_locale_h
#  define CC_no_stdarg_h
#  define CC_no_stddef_h
#  define CC_no_stdlib_h
#  define CC_broken_ieeetrap
#  endif


/*
 * :: RS 6000 platforms
 */
#if defined(CC_XLC) && defined(OS_AIX_RS)
#  define CC_CONFIGURED
#  ifdef OS_AIX41_RS
#    define CC_getcwd_has_proto
#    define CC_chdir_has_proto
#    define CC_execlp_has_proto
#    endif
#  endif

#if defined(CC_GNU) && defined(OS_AIX_RS)
#  define CC_CONFIGURED
#  endif

#if defined(CC_UNKNOWN) && defined(OS_AIX_RS)
#  define CC_CONFIGURED
#  endif


/*
 * :: IBM 370 platforms
 */
#if defined(CC_C370) && defined(OS_CMS)
#  define CC_CONFIGURED
#  define CC_non_ieee_floats
#  define CC_ebcdic_chars
#  endif

#if defined(CC_METAWARE) && defined(OS_AIX_370)
#  define CC_CONFIGURED
#  define CC_non_ieee_floats
#  define CC_no_enum_typedef
#  define CC_nlchar_unused_static
#  define CC_broken_toupper
#  define CC_undecl_flsbuf
#  define CC_missing_memmove
#  endif

#if defined(CC_METAWARE) && defined(OS_AIX_ESA)
#  define CC_CONFIGURED
#  define CC_non_ieee_floats
#  define CC_no_stdarg_h
#  endif


/*
 * :: Sun platforms
 */

#if defined(HW_SPARC_64)
#  define CC_second_largest_is_int
#  define CC_long_not_int32
#  define CC_SF_is_double
#endif

#if defined(OS_SUN)
#  if defined(OS_SUNOS4)
#    if defined(CC_GNU)

#      define CC_missing_memmove
#      define CC_noncc_sprintf
#      define CC_stdlib_no_exits
#      define CC_math_wants_posix
#      define CC_CONFIGURED

#    elif defined(CC_SUN)

#      define CC_no_const
#      define CC_sun_sparc_varargs
#      define CC_no_float_h
#      define CC_no_stdarg_h
#      define CC_missing_remove
#      define CC_missing_memmove
#      define CC_undecl_flsbuf
#      define CC_broken_toupper
#      define CC_noncc_sprintf
#      define CC_stdlib_no_exits
#      define CC_math_wants_posix
#      define CC_CONFIGURED

#    else 
#      error unknown SunOS4 compiler
#    endif /* CC_GNU */
#  elif defined(OS_SUNOS5)
#    if defined(CC_GNU)
#      define CC_noisy_ctype
#      define CC_CONFIGURED
#    elif defined(CC_SPARCCOMPILER)
#      define CC_CONFIGURED
#    else 
#      error unknown SunOS5 compiler
#    endif /* CC_GNU */
#  else 
#    error SunOS version is not 4 or 5 
#  endif /* OS_SUNOS4 */
#endif /* OS_SUN */

/*
 * :: 64-bit DEC platforms
 */
#if defined(CC_DEC_AXP) && defined(HW_AXP) && defined(OS_UNIX)
#  define CC_CONFIGURED
#  define CC_little_endian
#  define CC_second_largest_is_int
#  define SF_no_denorms
#  endif

#if defined(CC_DEC_AXP) && defined(HW_AXP) && defined(OS_VMS)
#  define CC_CONFIGURED
#  define CC_little_endian
#  define CC_second_largest_is_int
#  endif

/*
 * MIPS platforms
 */
#if defined(CC_MIPS) && defined(OS_IRIX) && defined (HW_MIPS)
#  define CC_getcwd_has_proto
#  if HW_MIPS==4
/* this is long64 int32 ptr64 */
#    define CC_second_largest_is_int
#    endif
#  define CC_CONFIGURED
#  endif

/*
 * :: NeXT platforms
 */
#if defined(OS_NEXT)
#  define CC_CONFIGURED
#  define CC_noncc_fprintf
#  endif

/*
 * :: Mac platforms
 */

#if defined(__APPLE__) && defined(__GNUC__) && !defined(__ppc__)
#define CC_little_endian
#endif


/*
 * :: HP platforms
 */

#if defined(OS_HPUX) /* may want to split the gnu case */
#  define CC_CONFIGURED
#  define CC_hpux_broken_stdarg
#  define CC_no_math_h
#  endif

#if defined(HW_IA64)
#  define CC_CONFIGURED
#  define CC_second_largest_is_int
#  define CC_long_not_int32
#endif

/*
 * :: Unknown platforms
 */
#if !defined(CC_CONFIGURED) && !defined(__STDC__)
#  define CC_CONFIGURED
#  define CC_no_void_pointer
#  define CC_no_const
#  define CC_no_float_h
#  define CC_no_limits_h
#  define CC_no_locale_h
#  define CC_no_stdarg_h
#  define CC_no_stddef_h
#  define CC_no_stdlib_h
#  define CC_missing_memmove
#  endif

#endif /* !_CCONFIG_H_ */
