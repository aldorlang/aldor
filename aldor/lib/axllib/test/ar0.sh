#!/bin/sh
#
# This script tests archive not found.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

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
