AC_DEFUN([ALDOR_HAS_CC_OPTION],
[AC_MSG_CHECKING($CC supports $1); echo "$3" > conftest$1.c;
 res=no
 if $CC -Werror $1 -c conftest$1.c > /dev/null 2>&1; then $2=1; res=yes; fi;
 AC_MSG_RESULT($res)])

AC_DEFUN([ALDOR_CC_STD],
[AC_MSG_CHECKING($CC supports gnu17)
 ALDOR_CC_OPTION(-std=gnu17,cfg_gnu17)
 if test "" = "$cfg_gnu17"; then cfg_has_gnu17=1; else cfg_has_gnu17=0; fi
 AC_DEFINE_UNQUOTED([HAVE_STD_GNU17], [$cfg_has_gnu17], [Use -std=gnu17 in unicl])
 CFLAGS="$CFLAGS $cfg_gnu17"])

