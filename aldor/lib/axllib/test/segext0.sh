#!/bin/sh
#
# This script tests finding '..' from Segment SingleInteger.

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling segext[012].as.'
$ALDOR -laxllib -R $TMPDIR -Y $TMPDIR -lAxiomLib=ax0 segext0.as segext1.as segext2.as

echo '== The files are:'
(cd $TMPDIR ; ls segext*.ao)

echo '== Cleaning up'
rm $TMPDIR/segext*.ao
