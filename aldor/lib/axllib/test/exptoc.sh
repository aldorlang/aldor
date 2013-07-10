#!/bin/sh
#
# This script tests exporting an aldor function to a C file

SRC=`pwd`
# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}
cd $TMPDIR

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	EXE=.exe
	OBJ=obj
	UNICL=unicl.sh
	LM=
else
	EXE=
	OBJ=o
	UNICL=unicl
	LM=-lm
fi

echo '== Compiling expfact.c into expfact.o'
$UNICL -c -I$ALDORROOT/include $SRC/expfact.c > /dev/null

echo '== Compiling exptoc.as into exptoc executable'
cp $SRC/exptoc.as .
aldor $LM -Mno-ALDOR_W_CantUseArchive -laxllib -F x exptoc.as expfact.$OBJ

echo '== Executing exptoc'
./exptoc

echo '== Cleaning up'
rm -f expfact.$OBJ exptoc.as exptoc$EXE

echo '== Done'
