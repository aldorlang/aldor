#!/bin/sh

TESTDIR=$ALGEBRAROOT/test
if [ ! -d $TESTDIR ]
then
	echo "Sorry: cannot find $TESTDIR"
	exit 1
fi

cd $TESTDIR
echo "Running tests in $TESTDIR with -q1 -Qno-del-assert ... "
for i in ./*test.as ; do (aldor -lalgebra -laldor -y$ALDORLIBROOT/lib -y$ALGEBRAROOT/lib -q1 -Grun -Qno-del-assert $i); done
/bin/rm -f $TESTDIR/*test.ao
echo "... done."
/bin/rm -f $TESTDIR/sm_polring0.test.as
#echo "Running tests in $TESTDIR with -q3 -Qno-del-assert ... "
#for i in ./*test.as ; do (aldor -lalgebra -laldor -y$ALDORLIBROOT/lib -y$ALGEBRAROOT/lib -q3 -Grun -Qno-del-assert $i); done
#/bin/rm -f $TESTDIR/*test.ao
#echo "... done."
#echo "Running tests in $TESTDIR with -q5 -qinline-all -Qno-del-assert ... "
#for i in ./*test.as ; do (aldor -lalgebra -laldor -y$ALDORLIBROOT/lib -y$ALGEBRAROOT/lib -q5 -Grun -qinline-all -Qno-del-assert $i); done
#/bin/rm -f $TESTDIR/*test.ao
#echo "... done."

