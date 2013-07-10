#!/bin/sh

LIBDIR1=$ALGEBRAROOT/lib
DIR2=$HOME/lib/algebra/distrib
DIR3=$ALDORROOT/lib

if [ ! -d $LIBDIR1 ]
then
	echo "Sorry: cannot find $LIBDIR1"
	exit 1
fi
if [ ! -d $DIR3 ]
then
	echo "Sorry: cannot find $DIR3"
	exit 1
fi

if [ ! -d $DIR2 ]
then
     LIBDIR2=$DIR3
else 
     LIBDIR2=$DIR2
fi

echo "Uninstall: moving archives from $LIBDIR2 to $LIBDIR1 ... "

rm -f $LIBDIR1/libalgebra*
mv $LIBDIR2/libalgebra* $LIBDIR1/
echo "... done."