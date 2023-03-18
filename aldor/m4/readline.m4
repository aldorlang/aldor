# Select readline if available and wanted

AC_DEFUN([ALDOR_READLINE_OPTION],
  [AC_ARG_ENABLE(
     [edit-gloop],
     [AS_HELP_STRING([--enable-edit-gloop],[Allow editing in -gloop repl])],
     [case "${enableval}" in
      	   yes) edit_gloop=yes;;
	   no) edit_gloop=no;;
	   *) AC_MSG_ERROR([bad value ${enableval} for --enable-edit-gloop])
	   esac],
     [AC_MSG_NOTICE([no gloop edit option supplied (delete me later)])
      ])])
	
AC_DEFUN([ALDOR_READLINE_SELECT],
  [AC_MSG_CHECKING(edit-gloop is selected..);
     if test -n "$ax_cv_lib_readline"; then
       if test "x$edit_gloop" = xyes || test "x$edit_gloop" = x ; then
           AC_MSG_RESULT([yes])
	   AC_DEFINE(USE_GLOOP_SHELL, 1, [Use readline library in gloop])
	   gloop_shell=yes;
       else
           AC_MSG_RESULT([available, but not selected])
	   gloop_shell=no;
       fi;
     else
        if test "x$edit_gloop" = xyes; then
	   AC_MSG_FAILURE([No readline available])
	else
          gloop_shell=no;
	  AC_MSG_RESULT([not available])
	  fi;
     fi])

