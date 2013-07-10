#!/bin/sh

LIBDIR1=$ALGEBRAROOT/lib
DIR2=$HOME/lib/algebra/distrib
DIR3=$ALDORROOT/lib

if [ ! -d $LIBDIR1 ]
then
	echo "Sorry: cannot find $LIBDIR1"
	exit 1
fi
if [ ! -d $DIR3 ]
then
	echo "Sorry: cannot find $DIR3"
	exit 1
fi

if [ ! -d $DIR2 ]
then
     LIBDIR2=$DIR3
else 
     LIBDIR2=$DIR2
fi

echo "Installing archives from $LIBDIR1 in $LIBDIR2 ... "

rm -f $LIBDIR2/libalgebra*
rm -f $LIBDIR1/stamp-*
mv $LIBDIR1/libalgebra* $LIBDIR2/
cd $LIBDIR1
if ([ ! -f $LIBDIR2/libalgebra.a  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebra.a"
else
      ln -s $LIBDIR2/libalgebra.a
fi
if ([ ! -f $LIBDIR2/libalgebra.al  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebra.al"
else
      ln -s $LIBDIR2/libalgebra.al
fi
if ([ ! -f $LIBDIR2/libalgebrad.a  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebrad.a"
else
      ln -s $LIBDIR2/libalgebrad.a
fi
if ([ ! -f $LIBDIR2/libalgebrad.al  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebrad.al"
else
      ln -s $LIBDIR2/libalgebrad.al
fi
if ([ ! -f $LIBDIR2/libalgebra-gmp.a  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebra-gmp.a"
else
      ln -s $LIBDIR2/libalgebra-gmp.a
fi
if ([ ! -f $LIBDIR2/libalgebra-gmp.al  ]) 
then 
      echo "Archive missing: $LIBDIR2/libalgebra-gmp.al"
else
      ln -s $LIBDIR2/libalgebra-gmp.al
fi
echo "... done."