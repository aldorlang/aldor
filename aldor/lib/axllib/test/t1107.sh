#!/bin/sh
#

SRC=`pwd`
SINK=/dev/null
cd $TMPDIR

cp $SRC/t1107a.as $TMPDIR
cp $SRC/t1107b.as $TMPDIR

aldor t1107a.as t1107b.as
aldor t1107a.as
aldor t1107b.as
