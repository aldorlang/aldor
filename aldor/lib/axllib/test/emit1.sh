#!/bin/sh
#
# This script tests the use of temporary files and warning messages in emit.c.

SRC=`pwd`
cd ${TMPDIR-/tmp}

# Make sure the generated lisp code doesn't contain a main expression.
aldor.sh -Zdb -F c $SRC/test0.as
mv test0.c test0.c.bak

aldor.sh -Zdb $SRC/test0.as
aldor.sh -Zdb -F c test0.ao

diff test0.c.bak test0.c
rm -f test0.c.bak test0.c test0.ao
