#!/bin/sh
#

SRC=`pwd`
SINK=/dev/null
cd $TMPDIR

cp $SRC/t1070a.as $TMPDIR
cp $SRC/t1070b.as $TMPDIR

aldor -laxllib -fo -fao t1070a.as
aldor -laxllib -ginterp t1070b.as
aldor -laxllib -grun t1070b.as t1070a.o

aldor -q3 -laxllib -fo -fao t1070a.as
aldor -q3 -laxllib -ginterp t1070b.as
aldor -q3 -laxllib -grun t1070b.as t1070a.o
