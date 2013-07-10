#!/bin/sh
#
# This script tests finding UFD operations from INS.

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling intcat0.as and intcat1.as'
$ALDOR -R $TMPDIR -Y $TMPDIR intcat0.as intcat1.as

echo '== The files are:'
(cd $TMPDIR ; ls intcat*.ao)

echo '== Cleaning up'
rm $TMPDIR/intcat*.ao
