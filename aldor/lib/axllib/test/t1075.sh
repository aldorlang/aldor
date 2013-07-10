#!/bin/sh
#

SRC=`pwd`
SINK=/dev/null
cd $TMPDIR

cp $SRC/t1075a.as $TMPDIR
cp $SRC/t1075b.as $TMPDIR
cp $SRC/t1075c.as $TMPDIR


aldor -q1 t1075a.as t1075b.as t1075c.as 
