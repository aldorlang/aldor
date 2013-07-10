#!/bin/sh
#
# This script tests the aldor -Fc++ command line argument.

S=`pwd`
if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	SRC=`cygpath -m $S`
	S=$SRC
	ALDOR=aldor.sh
	OBJ=obj
	EXE=.exe
else
	P=
	ALDOR=aldor
	OBJ=o
	SRC=$S
	EXE=
fi

SINK=/dev/null
cd ${TMPDIR-/tmp}
rm -f *ao
if [ "$CPP" = "" ]
then CPP=g++
fi

echo "== Generating C++ from db.as"
$ALDOR -Fc++ $SRC/db.as >> $SINK
echo "-- The files are:" db_*
for f in db_* ; do
  echo "**************************" $f
  cat $f
done

echo "== Compiling generated C++ "
$ALDOR -I$S -fo db_as.as
ls db_as.as db_as.$OBJ db_cc.h|sed "s/\.obj/.o/"

echo "== Linking program"
if [ "$P" = "win" ]; then
	cl /nologo /I. /I$ALDORROOT/include /Tp$SRC/appli1.C db_as.obj /Feappli1.exe /link $ALDORROOT/lib/libaxllib.lib $ALDORROOT/lib/libfoam.lib >>$SINK
else
	$CPP -I. -I$ALDORROOT/include $SRC/appli1.C db_as.o -L$ALDORROOT/lib -laxllib -lfoam -o appli1 >>$SINK
fi
echo "== Running program"
./appli1$EXE
echo "== Removing all output files."
rm -f db_* appli1$EXE

