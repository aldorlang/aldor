#!/bin/sh

LIBAL=$ALGEBRAROOT/lib/libalgebra-gmp.al
LIBA=$ALGEBRAROOT/lib/libalgebra-gmp.a
if [ ! -f $LIBAL ]
then
	echo "Sorry: cannot find $LIBAL"
	exit 1
fi

echo "Building $LIBA ... "

/bin/rm -f $LIBA

TMPDIR=/tmp/mkalgebra.$$
trap '/bin/rm -rf $TMPDIR; exit 1' 1 2 3 5 10 13 15

mkdir $TMPDIR || exit 1
cd $TMPDIR

for file in `uniar t $LIBAL`
do
   uniar x $LIBAL $file
   aldor -fo *.ao
   uniar rv $LIBA *.o
   /bin/rm -f *.ao *.o
done

ranlib $LIBA
cd /tmp
/bin/rm -rf $TMPDIR
echo "... done."
exit 0
