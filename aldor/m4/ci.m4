AC_DEFUN([ALDOR_CI_PREREQ],
 [AC_ARG_VAR([YC], [Yaml/json conversion tool])
  AC_CHECK_PROG([YQ],[yq],[yq])
  if test -z "$YQ"; then ci_check=no; else ci_check=yes; fi;
  if test "$ci_check" = yes; then
    AC_MSG_NOTICE([Will check git workflow files using yq])
  fi]
  [AM_CONDITIONAL(CI_CHECK, test "$ci_check" = yes)])

dnl use of AC_ARG_VAR is encouraged so that the value is persisted across sessions
