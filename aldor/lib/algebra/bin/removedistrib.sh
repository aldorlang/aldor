#!/bin/sh

DISTRIB=$ALDOR_DISTRIB/generic/$ALDOR_VERSION

if [ ! -d $DISTRIB ]
then
	echo "Sorry: could not find $DISTRIB"
	exit 1
else 
        echo "Removing distribution tree of the algebra library from $DISTRIB ..."
fi

DISTRIBLIB=$DISTRIB/lib
DISTRIBBIN=$DISTRIB/bin
DISTRIBINC=$DISTRIB/include
DISTRIBTEST=$DISTRIB/test.algebra

rm -fr $DISTRIBBIN/*algebra*
rm -fr $DISTRIBLIB/*algebra*
rm -fr $DISTRIBINC/*algebra*
rm -fr $DISTRIBTEST
rm -f  $DISTRIB/Makefile.algebra
rm -f  $DISTRIB/README.algebra

echo "... done."



