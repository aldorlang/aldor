#!/bin/sh

CURDIR=`pwd`
TMPDIR=/tmp/footest-tmp-$$

mkdir $TMPDIR
cd $TMPDIR
cat << EOF > file_with_a_long_name.as
#include "axllib"

foo(x:String):String == x;
EOF
cat << EOF > another_file_with_a_long_name.as
#include "axllib"

bar(x:String):String == x;
EOF
cat << EOF > footest.as
#include "axllib"
#library FooLib "foo"
import from FooLib;

import from String;
print << (foo bar "No problems!") << newline;
EOF
aldor -Fao file_with_a_long_name.as another_file_with_a_long_name.as
${AR} rcv libfoo.al file_with_a_long_name.ao another_file_with_a_long_name.ao 2>&1 | grep -v 'ar: writing'
echo ""
echo "-----------------------------------------------------------------"
aldor -ginterp -lfoo footest.as
echo "-----------------------------------------------------------------"
cd $CURDIR
/bin/rm -rf $TMPDIR
exit 0
