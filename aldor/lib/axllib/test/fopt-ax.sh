#!/bin/sh
#
# This script tests aldor -Fax option.

rm -f $TMPDIR/test0.ax

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling test0.as to $TMPDIR/test0.ax'
$ALDOR -R $TMPDIR -F ax test0.as > /dev/null
cat $TMPDIR/test0.ax
rm  $TMPDIR/test0.ax
