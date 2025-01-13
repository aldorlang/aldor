# Language/implementation features

# Co-routine based generators

AC_DEFUN([ALDOR_GENERATOR_OPTION],
  [AC_ARG_WITH([generators],
		[AS_HELP_STRING([--with-generators],[generator implementation (values: function, coroutine)])],
		[case "${withval}" in
		    function) generator=function;;
		    coroutine) generator=coroutine;;
		    *) AC_MSG_ERROR([bad value for 'enable-generator': ${withval}])
		    esac]
		[AC_MSG_NOTICE([generators: $generator])],
		[AC_MSG_NOTICE([no generator option provided])]
		)])

AC_DEFUN([ALDOR_GENERATOR_SELECT],
  [AC_MSG_CHECKING(generator implementation..);
   AM_CONDITIONAL([MK_GENERATOR_COROUTINES], [test x"$generator" = xcoroutine])
   if test x"$generator" = xcoroutine; then
       AC_MSG_RESULT([coroutines])
       AC_DEFINE(GENERATOR_COROUTINES, 1, [Use coroutines as implementation for generators])
   else
       AC_DEFINE(GENERATOR_COROUTINES, 0, [Use functions as implementation for generators])
       AC_MSG_RESULT([functions])
   fi])
