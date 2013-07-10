#!/bin/sh

TESTDIR=$ALGEBRAROOT/test
LIBDIR=$ALGEBRAROOT/lib

if [ ! -d $TESTDIR ]
then
	echo "Sorry: could not find $TESTDIR"
	exit 1
fi
if [ ! -d $LIBDIR ]
then
	echo "Sorry: could not find $LIBDIR"
	exit 1
fi

echo "Cleaning source tree for the algebra library ... "

cd $ALGEBRAROOT
rm -f *~
rm -f */*~
rm -f $LIBDIR/*
/bin/rm -f $TESTDIR/*test.as
/bin/rm -f $TESTDIR/*test
/bin/rm -f $TESTDIR/*stamp

echo "... done."



