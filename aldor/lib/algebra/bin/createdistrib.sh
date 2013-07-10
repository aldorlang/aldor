#!/bin/sh

export ALGEBRAROOT=$1
SRCDIR=$ALGEBRAROOT/src
LIBDIR=$ALGEBRAROOT/lib
INCDIR=$ALGEBRAROOT/include
BINDIR=$ALGEBRAROOT/bin
TESTDIR=$ALGEBRAROOT/test
DISTRIB=$ALDOR_DISTRIB/generic/$ALDOR_VERSION
export INCPATH=$ALGEBRAROOT/include:$DISTRIB/include
export LIBPATH=$ALGEBRAROOT/lib:$DISTRIB/lib

mkdir -p $DISTRIB

if [ ! -d $DISTRIB ]
then
	echo "Sorry: could not create $DISTRIB"
	exit 1
else 
        echo "Starting distribution tree for the algebra library in $DISTRIB"
fi

DISTRIBLIB=$DISTRIB/lib
DISTRIBBIN=$DISTRIB/bin
DISTRIBINC=$DISTRIB/include
DISTRIBTEST=$DISTRIB/test/algebra

echo "Cleaning directory $DISTRIB ..."
rm -fr $DISTRIBBIN/*algebra*
rm -fr $DISTRIBLIB/*algebra*
rm -fr $DISTRIBINC/*algebra*
rm -fr $DISTRIBTEST
rm -f  $DISTRIB/Makefile.algebra
rm -f  $DISTRIB/README.algebra
echo "... done."

echo "Creating subdirectories in $DISTRIB ..."
mkdir -p $DISTRIBLIB
mkdir -p $DISTRIBINC
mkdir -p $DISTRIBBIN
mkdir -p $DISTRIBTEST
echo "... done."

MAKE=gmake

echo "Installing libalgebra.al in $DISTRIBLIB ..."
if ( [  ! -f $LIBDIR/libalgebra.al  -o  -L $LIBDIR/libalgebra.al  ] )
then
        rm -f $LIBDIR/libalgebra.al
	cd $SRCDIR
        $MAKE release
fi
cp -f $LIBDIR/libalgebra.al $DISTRIBLIB/
echo "... done."

echo "Installing libalgebrad.al in $DISTRIBLIB ..."
if ( [  ! -f $LIBDIR/libalgebrad.al  -o  -L $LIBDIR/libalgebrad.al  ] )
then
        rm -f $LIBDIR/libalgebrad.al
	cd $SRCDIR
        $MAKE debug
fi
cp -f $LIBDIR/libalgebrad.al $DISTRIBLIB/
echo "... done."

echo "Installing libalgebra-gmp.al in $DISTRIBLIB ..."
if ( [  ! -f $LIBDIR/libalgebra-gmp.al  -o  -L $LIBDIR/libalgebra-gmp.al  ] )
then
        rm -f $LIBDIR/libalgebra-gmp.al
	cd $SRCDIR
        $MAKE gmp
fi
cp -f $LIBDIR/libalgebra-gmp.al $DISTRIBLIB/
echo "... done."

echo "Copying include files in $DISTRIBINC ... "
cp -f $INCDIR/algebra.as $DISTRIBINC/
cp -f $INCDIR/algebrauid.as $DISTRIBINC/
echo "... done."

echo "Copying shell scripts in $DISTRIBBIN ..."
cp -f $BINDIR/makealgebra.sh $DISTRIBBIN/
cp -f $BINDIR/runtestalgebra.sh $DISTRIBBIN/
echo "... done."

echo "Copying test files in $DISTRIBTEST ..."
cp -f $TESTDIR/*test.as $DISTRIBTEST
rm -f $DISTRIBTEST/sit_modgcd.test.as
echo "... done."

# cp -f $ALGEBRAROOT/Makefile.algebra $DISTRIB/Makefile.algebra
# cp -f $ALGEBRAROOT/README.algebra $DISTRIB/README.algebra
cp -f $ALGEBRAROOT/Makefile.algebra $DISTRIB/Makefile
cp -f $ALGEBRAROOT/README.algebra $DISTRIB/README




