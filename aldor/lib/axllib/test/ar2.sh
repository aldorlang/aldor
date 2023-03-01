#!/bin/sh
#
# This script tests archive member not found.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

rm -rf $TMPDIR/lib
mkdir $TMPDIR/lib

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
	OBJ=obj
else
	ALDOR=aldor
	OBJ=o
fi

echo '== Compiling triv*.as into triv*.ao and triv*.o'
$ALDOR -R $TMPDIR -F ao -F o triv[0-3].as 2>&1 | grep -v 'warning: conflicting types for built-in function' | grep -v "GC:"

echo '== Building an archive containing triv*.ao'
${AR} cr $TMPDIR/lib/libtriv.al $TMPDIR/triv*.ao
rm -f $TMPDIR/triv*.ao

echo '== Building an archive containing triv*.o'
${AR} cr $TMPDIR/lib/libtriv.a $TMPDIR/triv*.$OBJ
rm -f $TMPDIR/triv*.$OBJ

echo '== Creating a client for the archive'
cat << END_libarch.as > $TMPDIR/libarch.as

-- A minimal program depending on archives of compiler libraries

#library Triv "triv4.ao"

import from Triv;

printTriv();

END_libarch.as

echo '== Testing that the client imports from the archive'
cd $TMPDIR
$ALDOR -Ccc=unicl.sh -Grun -Y $TMPDIR/lib -ltriv libarch.as

echo '== Done'
