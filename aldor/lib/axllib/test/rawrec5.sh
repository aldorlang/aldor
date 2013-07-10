#!/bin/sh
#
# This script tests passing raw records to C.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	OBJ=obj
	LM=
else
	P=
	OBJ=o
	LM=-lm
fi

echo '== Creating temporary copies ...'
cp rawrec5.as $TMPDIR
cp cpxadd.C $TMPDIR

echo '== Moving to temporary directory ...'
cd $TMPDIR

echo '== Compiling C file'
if [ "$P" = "win" ]; then
	unicl -c cpxadd.C > /dev/null
else
	docc -c -x c cpxadd.C > /dev/null
fi

echo '== Running Aldor program ...'
aldor $LM -Mno-ALDOR_W_CantUseArchive -Grun -Q3 -laxllib rawrec5.as cpxadd.$OBJ 2>&1 | grep -v 'makes integer from pointer without a cast' | grep -v 'rawrec5.c: In function'

echo '== Cleaning up ...'
/bin/rm -f rawrec5.as
/bin/rm -f cpxadd.C
/bin/rm -f cpxadd.$OBJ

echo '== Done!'

