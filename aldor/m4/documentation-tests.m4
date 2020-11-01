AC_DEFUN([ALDOR_DOCUMENTATION_TESTS],
[AC_CHECK_PROG([LATEX],[latex], [ok])
 AC_CHECK_PROG([PDFLATEX],[pdflatex], [ok])
 AC_CHECK_PROG([MAKEINDEX],[makeindex], [ok])
 AC_CHECK_PROG([FIG2EPS],[fig2eps],[ok])
 if test -z "$LATEX" -o -z "PDFLATEX" -o -z "$MAKEINDEX" -o -z "$FIG2EPS"; then docs_ok=no; fi;
 if test "$enable_documentation" = "if_exists" -a "$docs_ok" = "no"; then
    AC_MSG_NOTICE([Documentation disabled as prerequisite programs not found])
    DOCS=
 elif test "$enable_documentation" = "if_exists"; then
    AC_MSG_NOTICE([documentation enabled])
    DOCS=true
 elif test "$enable_documentation" = "yes" -a "$docs_ok" = "no"; then
    AC_MSG_ERROR([missing documentation prerequisites])
 elif test "$enable_documentation" = yes; then
    DOCS=true
 fi]
 [AC_SUBST([DOCS])]
 [AM_CONDITIONAL(BUILD_DOCS,test "$DOCS" = true)])
