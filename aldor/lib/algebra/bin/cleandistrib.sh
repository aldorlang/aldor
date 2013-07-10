#!/bin/sh

DISTRIB=$ALDOR_DISTRIB/generic/$ALDOR_VERSION

if [ ! -d $DISTRIB ]
then
	echo "Sorry: could not find $DISTRIB"
	exit 1
else 
        echo "Cleaning distribution tree for the algebra library in $DISTRIB ..."
fi

DISTRIBLIB=$DISTRIB/lib
DISTRIBTEST=$DISTRIB/test/algebra

rm -f $DISTRIBLIB/*algebra*.a
rm -f $DISTRIBTEST/*.ao
rm -f $DISTRIBTEST/*.o 
rm -f $DISTRIBTEST/*.c
rm -f $DISTRIBTEST/*.test

echo "... done."



