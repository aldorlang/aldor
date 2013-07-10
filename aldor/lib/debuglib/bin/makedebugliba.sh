#!/bin/sh
#

LIBAL=$1
if [ "$MACHINE" = "win32msvc" ]; then
	ALDOR=aldor.sh
	LIBEXT="lib"
	OBJ="obj"
	#AR="lib"
	#ARFLAGS="/nologo `cygpath -m $LIBA`"
	#ARFLAGSFIRST="/nologo /OUT:`cygpath -m $LIBA`"
	AR="ar"
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	ARFLAGS="rv $LIBA"
	ARFLAGSFIRST=$ARFLAGS
elif [ "$MACHINE" = "win32gcc" ]; then
	ALDOR=aldor.sh
	LIBEXT="a"
	OBJ="o"
	AR="ar"
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	ARFLAGS="rv $LIBA"
else
	ALDOR=aldor
	LIBEXT="a"
	OBJ="o"
	AR="ar"
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	ARFLAGS="rv $LIBA"
fi


echo "*** Building $LIBA ***"

#unicl -c -O src/mndWrite.c
#ar rv $LIBA mndWrite.o
#rm -f mndWrite.o

# compile libaxllib.al into libaxllib.a
for i in `$AR t $LIBAL`; do (
	$AR x $LIBAL $i;
	$ALDOR -fo  -csmax=0 $i;
	rm -f $i;
	if [ "$MACHINE" = "win32msvc" ]; then
		if [ -e $LIBA ]; then 
			$AR $ARFLAGS `basename $i .ao`.$OBJ;
		else
			$AR $ARFLAGSFIRST `basename $i .ao`.$OBJ;
		fi
	else
		$AR $ARFLAGS `basename $i .ao`.$OBJ;
	fi
	rm -f `basename $i .ao`.$OBJ);
done
ranlib $LIBA
