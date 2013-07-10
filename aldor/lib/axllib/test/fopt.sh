#!/bin/sh
#
# This script tests the aldor -F command line argument.

SRC=`pwd`
SINK=/dev/null
cd $TMPDIR

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling test0.as to test0.ai'
$ALDOR -F ai $SRC/test0.as >> $SINK
echo '-- The files are:'
echo test0.*

echo '== Compiling test0.ai to test0.ax'
$ALDOR -F ax test0.ai >> $SINK
echo '-- The files are:'
echo test0.*

echo '== Compiling test0.ai to test0.fm'
$ALDOR -F fm test0.ai >> $SINK
echo '-- The files are:'
echo test0.*

echo '== Compiling test0.fm to test0.ao'
$ALDOR -F ao test0.fm >> $SINK
echo '-- The files are:'
echo test0.*

echo '== Leaving just .as and .ao files.'
rm -f test0.ai test0.ax test0.fm

echo '== Compiling test0.ao to test0.fm'
$ALDOR -F fm test0.ao >> $SINK
echo '-- The files are:'
echo test0.*

echo '== Removing all output files.'
rm -f test0.ao test0.fm

echo '== Compiling test0.as to test0.{ai,ax,fm}'
$ALDOR -F ai -F ax -F fm $SRC/test0.as >>$SINK
echo '-- The files are:'
echo test0.*

echo '== Try compiling test0.fm to test0.fm'
$ALDOR -F fm test0.fm

echo '== Try compiling test0.ai to test0.c with test0.{ax,fm} around'
$ALDOR -F c test0.ai

echo '== Cleaning up'
rm -f test0.*
