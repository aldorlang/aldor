#!/bin/sh
#
# This script tests exporting an aldor function to a C file

SRC=`pwd`
# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}
cd $TMPDIR

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	UNICL=unicl.sh
	EXE=.exe
	OBJ=obj
	LM=
else
	UNICL=unicl
	EXE=
	OBJ=o
	LM=-lm
fi

echo '== Compiling expquo.c into expquo.o'
$UNICL -c -I$ALDORROOT/include $SRC/expquo.c > /dev/null

echo '== Compiling exptoc2.as into exptoc2 executable'
cp $SRC/exptoc2.as .
aldor $LM -Mno-ALDOR_W_CantUseArchive -laxllib -F x exptoc2.as expquo.$OBJ

echo '== Executing exptoc2'
./exptoc2

echo '== Cleaning up'
rm -f expquo.$OBJ exptoc2.as exptoc2$EXE

echo '== Done'
