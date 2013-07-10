#!/bin/sh
#
# This script tests aldor -Faxlmain option.

rm -f $TMPDIR/axlmain.c $TMPDIR/aldormain.c

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling triv1.as to $TMPDIR/axlmain.c'
$ALDOR -R $TMPDIR -F axlmain triv1.as > /dev/null
if [ -f $TMPDIR/axlmain.c ]
then
   cat $TMPDIR/axlmain.c
   rm  $TMPDIR/axlmain.c
elif [ -f $TMPDIR/aldormain.c ]
then
   cat $TMPDIR/aldormain.c
   rm  $TMPDIR/aldormain.c
fi
