#!/bin/sh
#
# This script tests importing a C function as a packed map.

SRC=`pwd`
# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}
cd $TMPDIR

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	S=`cygpath -m $SRC`
	ALDOR=aldor.sh
	OBJ=obj
	LM=
	UNICL=unicl.sh
else
	ALDOR=aldor
	OBJ=o
	LM=-lm
	UNICL=unicl
fi

echo '== Compiling packfns.c into packfns.o'
$UNICL -c -I$ALDORROOT/include $SRC/packfns.c

echo '== Compiling pack0.as into pack0 executable'
cp $SRC/pack0.as .
$ALDOR -F x -I$SRC -l axllib $LM -Mno-ALDOR_W_CantUseArchive pack0.as packfns.$OBJ

echo '== Executing pack0'
./pack0

echo '== Cleaning up'
rm -f packfns.obj pack0.as pack0

echo '== Done'
