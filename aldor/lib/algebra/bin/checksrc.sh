#!/bin/sh

ALGEBRAROOT=$1
SRCDIR=$ALGEBRAROOT/src
LIBDIR=$ALGEBRAROOT/lib
INCDIR=$ALGEBRAROOT/include
BINDIR=$ALGEBRAROOT/bin
TESTDIR=$ALGEBRAROOT/test

echo "Checking directories and files"
if [ ! -d $TESTDIR ]
then
	echo "Sorry: cannot find $TESTDIR"
	exit 1
fi
if [ ! -d $LIBDIR ]
then
	echo "Sorry: cannot find $LIBDIR"
	exit 1
fi
if [ ! -d $SRCDIR ]
then
	echo "Sorry: cannot find $SRCDIR"
	exit 1
fi
if [ ! -d $INCDIR ]
then
	echo "Sorry: cannot find $INCDIR"
	exit 1
fi
if [ ! -d $BINDIR ]
then
	echo "Sorry: cannot find $BINDIR"
	exit 1
fi
if [ ! -f $BINDIR/makealgebra.sh ]
then
	echo "Sorry: cannot find $BINDIR/makealgebra.sh"
	exit 1
fi
if [ ! -f $BINDIR/makealgebrad.sh ]
then
	echo "Sorry: cannot find $BINDIR/makealgebrad.sh"
	exit 1
fi
if [ ! -f $BINDIR/makealgebra-gmp.sh ]
then
	echo "Sorry: cannot find $BINDIR/makealgebra-gmp.sh"
	exit 1
fi
if [ ! -f $BINDIR/maketestsuite.sh ]
then
	echo "Sorry: cannot find $BINDIR/maketestsuite.sh"
	exit 1
fi
if [ ! -f $BINDIR/runtest.sh ]
then
	echo "Sorry: cannot find $BINDIR/runtest.sh"
	exit 1
fi
if [ ! -f $BINDIR/runtestalgebra.sh ]
then
	echo "Sorry: cannot find $BINDIR/runtestalgebra.sh"
	exit 1
fi
if [ ! -f $BINDIR/makeinstall.sh ]
then
	echo "Sorry: cannot find $BINDIR/makeinstall.sh"
	exit 1
fi
if [ ! -f $BINDIR/cleansrc.sh ]
then
	echo "Sorry: cannot find $BINDIR/cleansrc.sh"
	exit 1
fi
if [ ! -f $BINDIR/makeuninstall.sh ]
then
	echo "Sorry: cannot find $BINDIR/makeuninstall.sh"
	exit 1
fi
if [ ! -f $BINDIR/createdistrib.sh ]
then
	echo "Sorry: cannot find $BINDIR/createdistrib.sh"
	exit 1
fi
if [ ! -f $BINDIR/checkdistrib.sh ]
then
	echo "Sorry: cannot find $BINDIR/checkdistrib.sh"
	exit 1
fi
if [ ! -f $BINDIR/cleandistrib.sh ]
then
	echo "Sorry: cannot find $BINDIR/cleandistrib.sh"
	exit 1
fi
if [ ! -f $BINDIR/removedistrib.sh ]
then
	echo "Sorry: cannot find $BINDIR/removedistrib.sh"
	exit 1
fi
echo "... done."



## echo "Fuck your bloody as's!"
## echo "Warning: Stop your computer immediately and call the police!"
## echo "Hello! I'm a nasty virus speading all over your computer!"


