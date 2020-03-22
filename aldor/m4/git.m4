# Force git build id.

AC_DEFUN([ALDOR_GIT_BUILD_ID],
[git_build_id=""
if test "$ld_has_build_id" = "";
then
    AC_MSG_FAILURE([Need to set ld_has_build_id])
fi
AC_ARG_ENABLE([git-build-id],
              [AS_HELP_STRING([--enable-git-build-id],
                              [Force git sha1 hash as build id])],
              [case "${enableval}" in
	          yes) gitid=true;;
		  no) gitid=false;;
		  *) AC_MSG_ERROR([bad value ${enableval} for --enable-git-build-id]) ;;
		  esac],
	      [if test -f $srcdir/../.git/config ; then gitid=true; else gitid=false; fi]
              [if test $gitid = true; then git_build_id=1; fi])

# Git SHA1 hash as ld build-id.
AC_MSG_CHECKING([build id])
if test yes = "$ld_has_build_id" && test 1 = "$git_build_id"; then
   VCSVERSION=`cd $srcdir; git rev-parse HEAD`
   build_id="-Wl,--build-id=0x$VCSVERSION"
   AC_MSG_RESULT([git: $VCSVERSION ld: yes])
elif test 1 = "$git_build_id"; then
   VCSVERSION=`cd $srcdir; git rev-parse HEAD`
   AC_MSG_RESULT([git: $VCSVERSION ld: no])
else
   VCSVERSION=`date +%Y%m%d`
   AC_MSG_RESULT([date: $VCSVERSION])
fi
AC_SUBST([VCSVERSION])
AC_SUBST([build_id])
])

