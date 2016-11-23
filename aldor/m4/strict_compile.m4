# Define warnings based on compiler (and version)

AC_DEFUN([ALDOR_STRICT_COMPILE],
  [AC_MSG_CHECKING(Strict options for C compiler)
  cfgSTRICTCFLAGS="-pedantic -std=c99 -Wall -Wextra -Werror -Wno-empty-body -Wno-enum-compare -Wno-missing-field-initializers -Wno-sign-compare -Wno-unused -Wno-unused-parameter -Wno-error=format -Wno-error=type-limits -Wno-error=strict-aliasing  -Wno-error=shift-negative-value -Wno-unused"

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
