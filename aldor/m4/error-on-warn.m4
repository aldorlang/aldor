
AC_DEFUN([ALDOR_ERROR_ON_WARN],
[AC_MSG_CHECKING(what extra warning flags to pass to the C compiler)
if test ${GCC}x = yesx
then
  warnFLAGS=
  CFLAGS="${CFLAGS} -Wno-unused"
  # other flags...
  # These next two are included in the GNOME_COMPILE_WARNINGS
  #warnFLAGS="${warnFLAGS} -Wmissing-prototypes"
  #warnFLAGS="${warnFLAGS} -Wmissing-declarations"
  #warnFLAGS="${warnFLAGS} -Werror-implicit-function-declaration" # In -Wall

  #error_on_warning_as_default="yes"

  AC_ARG_ENABLE(error-on-warning,
    [AS_HELP_STRING([--disable-error-on-warning],[disable treating compile warnings as errors])],
    [case "${enableval}" in
       yes) warnFLAGS="${warnFLAGS} -Werror" ; gnc_error_on_warning=yes ;;
       no)  gnc_error_on_warning=no ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-error-on-warning) ;;
     esac],
    [ if test "${error_on_warning_as_default}" = "yes"; then
        warnFLAGS="${warnFLAGS} -Werror";
        gnc_error_on_warning=auto
      else
        gnc_error_on_warning=no
      fi
    ])

  CFLAGS="${warnFLAGS} ${CFLAGS}"
else
  warnFLAGS=none
fi
AC_MSG_RESULT($warnFLAGS)
])
