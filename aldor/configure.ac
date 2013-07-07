#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([aldor],[1.1],[nowhere@nowhere])
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
#AC_HEADER_STDBOOL
#AC_TYPE_UID_T
AC_TYPE_INT32_T
#AC_TYPE_MODE_T
#AC_TYPE_OFF_T
#AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_DECLS([sys_siglist])
#AC_STRUCT_TIMEZONE
#AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ALLOCA
#AC_FUNC_FORK
#AC_FUNC_MALLOC
#AC_FUNC_MKTIME
#AC_FUNC_REALLOC
#AC_FUNC_STRCOLL
#AC_FUNC_STRERROR_R
#AC_FUNC_STRTOD

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
		 aldor/lib/java/Makefile
		 aldor/lib/java/src/Makefile
		 aldor/tools/Makefile
		 aldor/tools/unix/Makefile
		 lib/Makefile
		 lib/axllib/Makefile
		 lib/axllib/src/Makefile
		 lib/axllib/src/al/Makefile
		 lib/axllib/test/Makefile
		 lib/libaxldem/Makefile
		 lib/libaxldem/src/Makefile
		 lib/libaxldem/src/al/Makefile
		 lib/libaxldem/test/Makefile
		 lib/libax0/Makefile
		 lib/libax0/src/Makefile
		 lib/libax0/src/al/Makefile
		 lib/libax0/test/Makefile
		 lib/aldor/Makefile
		 lib/aldor/include/Makefile
		 lib/aldor/src/Makefile
		 lib/aldor/src/lang/Makefile
		 lib/aldor/src/base/Makefile
		 lib/aldor/src/arith/Makefile
		 lib/aldor/src/datastruc/Makefile
		 lib/aldor/src/util/Makefile
		 lib/aldor/src/test/Makefile
		 lib/aldor/src/gmp/Makefile
		 lib/algebra/Makefile
		 lib/algebra/src/Makefile
		 lib/algebra/src/util/Makefile
		 lib/algebra/src/numbers/Makefile
		 lib/algebra/src/extree/Makefile
		 lib/algebra/src/extree/operators/Makefile
		 lib/algebra/src/extree/parser/Makefile
		 lib/algebra/src/categories/Makefile
		 lib/algebra/src/basic/Makefile
		 lib/algebra/src/basic/compbug/Makefile
		 lib/algebra/src/mat/Makefile
		 lib/algebra/src/mat/gauss/Makefile
		 lib/algebra/src/mat/modular/Makefile
		 lib/algebra/src/mat/modular/compbug/Makefile
		 lib/algebra/src/mat/linalg/Makefile
		 lib/algebra/src/multpoly/multpolydata/Makefile
		 lib/algebra/src/multpoly/exponent/Makefile
		 lib/algebra/src/univpoly/categories/Makefile
		 lib/algebra/src/mat/linalg2/Makefile
		 lib/algebra/src/univpoly/Makefile
		 lib/algebra/src/univpoly/gcd/Makefile
		 lib/algebra/src/fraction/Makefile
		 lib/algebra/src/series/Makefile
		 lib/algebra/src/series/compbug/Makefile
		 lib/algebra/src/mat/linalg3/Makefile
		 lib/algebra/src/algext/Makefile
		 lib/algebra/src/polyfactorp/Makefile
		 lib/algebra/src/ffield/Makefile
		 lib/algebra/src/polyfactor0/Makefile
		 lib/algebra/src/multpoly/multpolycat/Makefile
		 lib/algebra/src/multpoly/multpolydom/Makefile
		 lib/algebra/src/multpoly/multpolypkg/Makefile
		 lib/algebra/src/multpoly/multpolytest/Makefile])
AC_OUTPUT

# Shared libraries
