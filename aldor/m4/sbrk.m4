
AC_DEFUN([ALDOR_SBRK_OPTION],
[AC_MSG_CHECKING(Determining source for sbrk)
cat > conftest_sbrk.c << EOF
#include <unistd.h>
int main() { sbrk(3); }
EOF
case ${host_os} in
  darwin*) sbrk_opt=_SBRK_NOT_NEEDED
  *)
      if ${CC} ${CFLAGS} ${cfgSTRICTCFLAGS} conftest_sbrk.c >&AS_MESSAGE_LOG_FD 2>&1;
      then
           sbrk_opt=_ALDOR_ANY_SBRK
      elif ${CC} ${CFLAGS} ${cfgSTRICTCFLAGS} -D_BSD_SOURCE conftest_sbrk.c >&AS_MESSAGE_LOG_FD 2>&1;
      then
           sbrk_opt=_BSD_SOURCE
      elif ${CC} ${CFLAGS} ${cfgSTRICTCFLAGS} -D_DEFAULT_SOURCE conftest_sbrk.c  >&AS_MESSAGE_LOG_FD 2>&1;
      then
          sbrk_opt=_DEFAULT_SOURCE
      else
           AC_MSG_FAILURE([No way to get sbrk()])
      fi
esac
SBRK_OPT=$sbrk_opt
AC_SUBST(SBRK_OPT)
AC_MSG_RESULT($sbrk_opt)
])
