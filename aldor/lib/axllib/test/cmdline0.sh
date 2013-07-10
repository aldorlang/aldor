#!/bin/sh
#
# This script tests the compiler command line.
# See also: multi.sh, fopt.sh, msg.sh

echo '== Test compiling with non-existent objects on command line =='

aldor foo.o

echo '== Test capitalized file types =='

echo '#include "triv0.as"' > FOO.AS
aldor -FC FOO.AS
ls FOO.*
rm -f FOO.AS  FOO.c
