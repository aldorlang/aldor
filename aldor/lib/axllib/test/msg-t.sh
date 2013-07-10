#!/bin/sh
#
# This script tests the low level message system.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

cd $TMPDIR
cp $ALDORROOT/lib/comsgpig.cat .
aldor -Wtest+msg
rm -f comsgpig.cat
