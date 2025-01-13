
AC_DEFUN([ALDOR_INTERP_TESTS_INIT],[disable_interp_tests=no])

AC_DEFUN([ALDOR_INTERP_TESTS_OPTION],
  [AC_ARG_ENABLE(
     [interpreter-tests],
     [AS_HELP_STRING([--disable-interpreter-tests], [Disable all tests via foam interpreter])],
     [run_interp=yes;
      case "${enableval}" in
      	   yes) run_interp=yes;;
	   no) run_interp=no;;
	   *) AC_MSG_ERROR([bad value ${enableval} for --run-interp-tests])
	   esac; set +x],
     [AC_MSG_NOTICE([no interpreter test option supplied])
     [run_interp=yes]
      ])])
	
dnl note that other functions can set disable_interp_tests
AC_DEFUN([ALDOR_INTERP_TESTS],
 [AC_MSG_CHECKING(run interpreter tests..);
  if test x"$run_interp" = xyes -a x"$disable_interp_tests" = xno; then run_tests=yes; else run_tests=no; fi;
  AM_CONDITIONAL(WITH_ALDOR_INTERP_TESTS, test x"$run_tests" = xyes)
  AC_MSG_RESULT([ Running interpreter tests: $run_tests .. disabled by other rules: $disable_interp_tests ])])
