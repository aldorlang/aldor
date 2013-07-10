#!/bin/bash
##########################################################
#                                                        #
# Creates libaldorX.a given libaldorX.al                 #
#                                                        #
# The Aldor Development Group (2001-2002)                #
#                                                        #
##########################################################

if [ $# != 3 ] 
then 
          echo "Usage: $0 \$ALGEBRAROOT \$LIBAL \$OPTS" 
          exit 1    
fi

# Variables
ALGEBRAROOT=$1
LIBDIR=$ALGEBRAROOT/lib
LIBAL=$LIBDIR/$2
OPTS=$3
LIBA=`dirname $2`/`basename $2 .al`.a
LIBA=$LIBDIR/$LIBA

# Initial checks
if [ ! -f $LIBAL ]
then
	echo "Sorry: cannot find $LIBAL"
	exit 1
fi

echo "Building $LIBA ... "

# remove previous libalgebra.a
/bin/rm -f $LIBA

# remove old temp
TMPDIR=/tmp/mkalgebra.$$
trap '/bin/rm -rf $TMPDIR; exit 1' 1 2 3 5 10 13 15

# create temp dir
mkdir $TMPDIR || exit 1
cd $TMPDIR

# create $LIBA
for file in `uniar t $LIBAL`
do
   uniar x $LIBAL $file
   aldor -fo $OPTS -csmax=0 *.ao
   uniar rv $LIBA *.o
   /bin/rm -f *.ao *.o
done

ranlib $LIBA
cd /tmp
/bin/rm -rf $TMPDIR
echo "... done."
exit 0
