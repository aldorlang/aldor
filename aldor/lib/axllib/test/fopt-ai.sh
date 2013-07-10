#!/bin/sh
#
# This script tests $ALDOR -Fi option.

rm -f $TMPDIR/triv1.ai

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling triv1.as to $TMPDIR/triv1.ai'
$ALDOR -R $TMPDIR -F ai triv1.as > /dev/null
cat $TMPDIR/triv1.ai
rm  $TMPDIR/triv1.ai
