#!/bin/sh
#
# This script tests the aldor -Fc++ command line argument.

SRC=`pwd`
if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	S=`cygpath -m $SRC`
	ALDOR=aldor.sh
	OBJ=obj
	EXE=.exe
else
	P=
	ALDOR=aldor
	OBJ=o
	EXE=
fi

SINK=/dev/null
cd ${TMPDIR-/tmp}
if [ "$CPP" = "" ] ; then CPP=g++ ; fi
echo "== Generating C++ from mylist.as"
$ALDOR -Fc++ $SRC/mylist.as >> $SINK
echo "-- The files are:" mylist_*
for f in mylist_* ; do
  echo "**************************" $f
  cat $f
done

echo "== Compiling generated C++ "
$ALDOR -I$SRC -fo mylist_as.as
ls mylist_as.as mylist_as.$OBJ mylist_cc.h|sed "s/\.obj/.o/"

echo "== Linking program"
if [ "$P" = "win" ]; then
	cl -nologo -I. -I$ALDORROOT/include /EHsc /Tp$S/appli2.C mylist_as.obj -Feappli2.exe -link $ALDORROOT/lib/libaxllib.lib $ALDORROOT/lib/libfoam.lib >> $SINK
else
	$CPP -Wno-deprecated -I. -I$ALDORROOT/include $SRC/appli2.C mylist_as.o -L$ALDORROOT/lib -laxllib -lfoam -o appli2
fi

echo "== Running program"
./appli2$EXE
echo "== Removing all output files."
rm -f mylist_* appli2$EXE

