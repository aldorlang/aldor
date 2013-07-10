#!/bin/sh
#
# This script tests finding CommRing operations from ComplexCat.

echo '== Compiling supcat0.as and supcat1.as'
aldor.sh -laxllib -R $TMPDIR -Y $TMPDIR supcat0.as supcat1.as

echo '== The files are:'
(cd $TMPDIR ; ls supcat*.ao)

echo '== Cleaning up'
rm $TMPDIR/supcat*.ao
