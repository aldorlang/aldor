#!/bin/sh

export ALGEBRAROOT=$1
TESTDIR=$ALGEBRAROOT/test
SRCDIR=$ALGEBRAROOT/src
if [ ! -d $TESTDIR ]
then
	echo "Sorry: cannot find $TESTDIR"
	exit 1
fi
if [ ! -d $SRCDIR ]
then
	echo "Sorry: cannot find $SRCDIR"
	exit 1
fi

echo "Building test suite for $ALGEBRAROOT ... "

/bin/rm -f $TESTDIR/*test.as
cd $SRCDIR
make test
cd $TESTDIR
for i in ./*test.as ; do (if ([ -z "`cat $i`" ]) then (rm -f $i); fi); done
for i in ./stamp* ; do (if ([ -z "`cat $i`" ]) then (rm -f $i); fi); done

echo "... done."
