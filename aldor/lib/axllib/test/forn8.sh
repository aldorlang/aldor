#!/bin/sh
#
# This script tests the aldor -Fc++ command line argument.

ALDOR=aldor.sh
SRC=`pwd`
SINK=/dev/null
cd $TMPDIR

echo "== Compiling forn8.as"
$ALDOR -Fc++ $SRC/forn8.as >> $SINK
echo "-- The files are:" forn8_*
for f in forn8_* ; do
  echo "**************************" $f
  cat $f
done

echo "== Removing all output files."
rm -f forn8_*

