#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([aldor], [1.1], [nowhere@nowhere])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([aldor/src/main.c])
AC_CONFIG_AUX_DIR([amaux])

# Automake
AM_MAINTAINER_MODE([enable])
AM_INIT_AUTOMAKE
AM_PROG_CC_C_O
AM_PROG_LEX
AC_PROG_YACC

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_LN_S

# Checks for libraries.
LT_INIT()
AC_SUBST([LIBTOOL_DEPS])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h fenv.h float.h mach/mach.h stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_INT32_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_DECLS([sys_siglist])
AC_STRUCT_TIMEZONE
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRCOLL
AC_FUNC_STRERROR_R
AC_FUNC_STRTOD

# Should only check what we need
#AC_CHECK_FUNCS([atexit bzero clock_gettime fesetround floor ftime getcwd isascii localeconv localtime_r mblen memchr memmove memset modf pow putenv realpath rint rpmatch select setenv setlocale sqrt stime strcasecmp strchr strcspn strdup strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoul strtoull tzset])
AC_CHECK_FUNCS([atexit])

# Just in case
GENERATED_WARNING="THIS IS A GENERATED FILE"
AC_SUBST([GENERATED_WARNING])

AC_CONFIG_FILES([Makefile
		 aldor/Makefile
		 aldor/subcmd/Makefile
		 aldor/subcmd/unitools/Makefile
		 aldor/subcmd/testaldor/Makefile
		 aldor/src/Makefile
		 aldor/lib/Makefile
		 aldor/lib/libfoamlib/Makefile
		 aldor/lib/libfoamlib/al/Makefile
		 aldor/lib/libfoam/Makefile
		 aldor/lib/libfoam/al/Makefile
		 aldor/test/Makefile
		 aldor/tools/Makefile
		 aldor/tools/unix/Makefile])
AC_OUTPUT

# Shared libraries
