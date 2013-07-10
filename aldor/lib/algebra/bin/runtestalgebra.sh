#!/bin/bash
##########################################################
#                                                        #
# Run the tests for libalgebra                           #
#                                                        #
# The Aldor Development Group (2001-2002)                #
#                                                        #
##########################################################

if ([ $# != 1 ]) then ( echo "Usage: $0 \$ALGEBRAROOT"; exit 1; ) fi

# Variables
export ALGEBRAROOT=$1

# Test directory
TESTDIR=$ALGEBRAROOT/test/algebra
if [ ! -d $TESTDIR ]
then
	echo "Sorry: cannot find $TESTDIR"
	exit 1
fi

# Run tests
echo "Running tests in $TESTDIR ... "
cd $TESTDIR
/bin/rm -f $TESTDIR/sm_polring0.test.as
for i in ./*test.as ; do (aldor -lalgebra -laldor -y$ALDORLIBROOT/lib -y$ALGEBRAROOT/lib -q0 -Grun -Qno-del-assert -qinline-all $i); done
/bin/rm -f $TESTDIR/*test.ao
echo "... done."

