# Define warnings based on compiler (and version)

AC_DEFUN([ALDOR_CC_OPTION],
[AC_MSG_CHECKING($CC supports $1); echo > conftest$1.c;
 res=no
 if $CC $1 -c conftest$1.c > /dev/null 2>&1; then $2="$1"; res=yes; fi;
 AC_MSG_RESULT($res)])

AC_DEFUN([ALDOR_STRICT_COMPILE],

  [ALDOR_CC_OPTION(-Wno-shift-negative-value,cfg_no_shift_negative_value)
   ALDOR_CC_OPTION(-Wno-sign-compare,cfg_no_sign_compare)
   AC_MSG_CHECKING(Strict options for C compiler)

   cfgSTRICTCFLAGS="-pedantic -std=c99 -Wall -Wextra -Werror -Wno-empty-body -Wno-enum-compare -Wno-missing-field-initializers -Wno-unused -Wno-unused-parameter -Wno-error=format -Wno-error=type-limits -Wno-error=strict-aliasing -Wno-unused $cfg_no_sign_compare $cfg_no_shift_negative_value "

   if test "${CC}x" = gccx
   then
       cfgSTRICTCFLAGS="${cfgSTRICTCFLAGS} -Wno-error=clobbered -Wno-unused"
   elif test "${CC}x" = clangx
   then
       cfgSTRICTCFLAGS="${cfgSTRICTCFLAGS} -fcolor-diagnostics -Wno-error=enum-conversion -Wno-error=tautological-compare -Wno-parentheses-equality"
   else
       AC_MSG_WARN(Unknown C compiler ${CC})
       cfgSTRICTCFLAGS=""
   fi
   AC_MSG_RESULT(${CC})])
