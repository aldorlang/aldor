# Define warnings based on compiler (and version)

AC_DEFUN([ALDOR_CC_OPTION],
[AC_MSG_CHECKING($CC supports $1); echo "$3" > conftest$1.c;
 res=no
 if $CC -Werror $1 -c conftest$1.c > /dev/null 2>&1; then $2="$1"; res=yes; fi;
 AC_MSG_RESULT($res)])

AC_DEFUN([ALDOR_STRICT_COMPILE],

  [ALDOR_CC_OPTION(-Wno-error=shift-negative-value,cfg_no_shift_negative_value,int main() { return -1 << 1; })
   ALDOR_CC_OPTION(-Wno-sign-compare,cfg_no_sign_compare)
   AC_MSG_CHECKING(Strict options for C compiler)

   cfgSTRICTCFLAGS="-pedantic -std=c99 -Wall -Wextra -Werror -Wno-empty-body -Wno-enum-compare \
                    -Wno-missing-field-initializers -Wno-unused -Wno-unused-parameter \
		    -Wno-error=format -Wno-error=type-limits -Wno-error=strict-aliasing \
		    $cfg_no_sign_compare $cfg_no_shift_negative_value"
   case "${CC}" in
       gcc*)
             cfgSTRICTCFLAGS="${cfgSTRICTCFLAGS} -Wno-error=clobbered"
	     ;;
       clang*)
             cfgSTRICTCFLAGS="${cfgSTRICTCFLAGS} -fcolor-diagnostics -Wno-error=enum-conversion \
				-Wno-error=tautological-compare -Wno-parentheses-equality"
	     ;;
       *)
             AC_MSG_WARN(Unknown C compiler ${CC})
             cfgSTRICTCFLAGS="";;
    esac
   AC_MSG_RESULT(${CC})])
