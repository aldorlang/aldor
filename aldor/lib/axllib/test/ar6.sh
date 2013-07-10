#!/bin/sh
#
# This script tests importing directly from an archive.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

rm -rf $TMPDIR/lib
mkdir $TMPDIR/lib

echo '== Compiling numeral*.as into numeral*.ao'
$ALDOR -R $TMPDIR -Y $TMPDIR -F ao numeral*.as | grep -v "GC:"

echo '== Building an archive containing numeral*.ao'
ar cr $TMPDIR/lib/libnum.al $TMPDIR/numeral*.ao
rm -f $TMPDIR/numeral*.ao

echo '== Creating a client for the archive'
cat << END_numeral.as > $TMPDIR/numeral.as

-- A minimal program importing directly from an archive of compiler libraries.
-- Note that this one also uses a keyed reference to a compiler library.

#include "axllib.as"
#library Numeral "num"

import from Numeral;

-- Zero and One are exported from numeral*.as. 
export Num: with {
        0: %;
        1: %;
}
== add {
        0: % == Zero pretend %;
        1: % == One  pretend %;
}

END_numeral.as

echo '== Testing that the client imports from the archive'
$ALDOR -WTt+tinfer -Y $TMPDIR/lib $TMPDIR/numeral.as

echo '== Done'
