#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([aldor],[1.2],[aldor@xinutec.org])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([aldor/src/main.c])
AC_CONFIG_AUX_DIR([amaux])

# Automake
AM_INIT_AUTOMAKE([foreign silent-rules parallel-tests])
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_LEX
AC_PROG_YACC

# Check for GMP
AC_CHECK_HEADER([gmp.h],
                [AM_CONDITIONAL(GMP, true)],
                [AM_CONDITIONAL(GMP, false)])

# Options
AC_ARG_ENABLE([libraries],
              [AS_HELP_STRING([--disable-libraries],
                              [Do not build Aldor libraries])],
              [AM_CONDITIONAL(BUILD_LIBS, false)],
              [AM_CONDITIONAL(BUILD_LIBS, true)])

AC_ARG_ENABLE([java],
              [AS_HELP_STRING([--disable-java],
                              [Do not build Aldor Java libraries])],
              [AM_CONDITIONAL(BUILD_JAVA, false)],
              [AM_CONDITIONAL(BUILD_JAVA, true)])

AC_ARG_WITH([boehm-gc],
            [AS_HELP_STRING([--with-boehm-gc],
                            [Use Boehm Garbage Collector instead of built-in])],
            [CFLAGS="$CFLAGS -DSTO_USE_BOEHM"
             LIBS="$LIBS -lgc"
            ])

# Checks for libraries.
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# Enable extra warnings and -Werror if supported.
ALDOR_ERROR_ON_WARN

# Git SHA1 hash as ld build-id.
AC_MSG_CHECKING([git build-id])
if test -z "`git status --porcelain 2>&1`"; then
   sha1=`git rev-parse HEAD`
   build_id="-Wl,--build-id=0x$sha1"
   AC_MSG_RESULT([$sha1])
else
   AC_MSG_RESULT([no git or tree not clean])
fi
AC_SUBST([build_id])

# Generate Makefiles
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
		 aldor/lib/java/Makefile
		 aldor/lib/java/src/Makefile
		 aldor/test/Makefile
		 aldor/tools/Makefile
		 aldor/tools/unix/Makefile
		 aldor/tools/unix/aldor
		 lib/Makefile
		 lib/axllib/Makefile
		 lib/axllib/src/Makefile
		 lib/axllib/src/al/Makefile
		 lib/axllib/test/Makefile
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
		 lib/algebra/include/Makefile
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
		 lib/algebra/src/multpoly/multpolytest/Makefile
		 lib/libaxldem/Makefile
		 lib/libaxldem/src/Makefile
		 lib/libaxldem/src/al/Makefile
		 lib/libaxldem/test/Makefile
		 lib/libax0/Makefile
		 lib/libax0/src/Makefile
		 lib/libax0/src/al/Makefile
		 lib/libax0/test/Makefile])
AC_OUTPUT
