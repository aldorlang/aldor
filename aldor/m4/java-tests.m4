# Find junit library, and if it exists, set JUNIT_JAR to its location
AC_DEFUN([ALDOR_JAVA_TESTS],
[AC_CHECK_FILE([$JUNIT_JAR],
		  [HAS_JUNIT=yes], [HAS_JUNIT=no])
AC_SUBST([JUNIT_JAR])
AC_SUBST([HAS_JUNIT])
AM_CONDITIONAL(HAS_JUNIT, test "x$enable_java" = xyes -a x"$HAS_JUNIT" != xno)
])
