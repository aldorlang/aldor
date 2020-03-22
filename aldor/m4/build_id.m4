# Check that ld supports build id
AC_DEFUN([ALDOR_LD_BUILD_ID_CHECK],
[AC_MSG_CHECKING([if the linker accepts -Wl,--build-id=none])
safe_CFLAGS=$CFLAGS
AC_LANG([C])
CFLAGS="-Wl,--build-id=none -Werror"
AC_LINK_IFELSE(
[AC_LANG_PROGRAM([ ], [return 0;])],
[
  ld_has_build_id=yes
  AC_MSG_RESULT([yes])
], [
  ld_has_build_id=no
  AC_MSG_RESULT([no])
])
CFLAGS=$safe_CFLAGS
])
