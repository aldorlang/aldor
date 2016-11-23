
AC_DEFUN([ALDOR_ERROR_ON_WARN],
ALDOR_STRICT_COMPILE
[AC_MSG_CHECKING(what extra warning flags to pass to the C compiler)
  warnFLAGS=
  STRICTCFLAGS="${CFLAGS}"

  error_on_warning_as_default="yes"

  AC_ARG_ENABLE(error-on-warning,
    [AS_HELP_STRING([--disable-error-on-warning],[disable treating compile warnings as errors])],
    [case "${enableval}" in
       yes) aldor_error_on_warning=yes ;;
       no)  aldor_error_on_warning=no ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-error-on-warning) ;;
     esac],
    [ if test "${error_on_warning_as_default}" = "yes"; then
        aldor_error_on_warning=auto-yes
      else
        aldor_error_on_warning=auto-no
      fi
    ])

  case "${aldor_error_on_warning}" in
    *yes)
         STRICTCFLAGS=${cfgSTRICTCFLAGS};;
    *no)
         STRICTCFLAGS=;;
    *);;
  esac
AC_SUBST(STRICTCFLAGS)
AC_MSG_RESULT($aldor_error_on_warning - ${STRICTCFLAGS})
])


