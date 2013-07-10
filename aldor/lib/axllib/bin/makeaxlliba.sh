#!/bin/sh
#

LIBAL=$1

if [ "$MACHINE" = "win32msvc" ]; then
	ALDOR="$ALDORROOT/bin/aldor.sh"
	UNICL="$ALDORROOT/bin/unicl.sh"
	LIBEXT="lib"
	OBJ="obj"
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	#AR="lib"
	#ARFLAGS="/nologo `cygpath -m $LIBA`"
	#ARFLAGSFIRST="/nologo /OUT:`cygpath -m $LIBA`"
	AR="ar"
	ARFLAGS="rv"
elif [ "$MACHINE" = "win32gcc" ]; then
	ALDOR="$ALDORROOT/bin/aldor.sh"
	UNICL="$ALDORROOT/bin/unicl.sh"
	LIBEXT="a"
	OBJ="o"
	LOB=$LIBDIR/libaxllib.$LIBEXT
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	AR="ar"
	ARFLAGS="rv"
else
	ALDOR="$ALDORROOT/bin/aldor"
	UNICL="$ALDORROOT/bin/unicl"
	LIBEXT="a"
	OBJ="o"
	LIBA=`dirname $LIBAL`/`basename $LIBAL .al`.$LIBEXT
	AR="ar"
	ARFLAGS="rv"
fi

echo "*** Building $LIBA ***"

$UNICL -c -O src/mndWrite.c
$AR $ARFLAGS $LIBA mndWrite.$OBJ
rm -f mndWrite.$OBJ

# compile libaxllib.al into libaxllib.a
for i in `$AR t $LIBAL`; do (
	$AR x $LIBAL $i;
	$ALDOR -fo -q5 -qinline-all -csmax=0 $i;
	rm -f $i;
	$AR $ARFLAGS $LIBA `basename $i .ao`.$OBJ;
	rm -f `basename $i .ao`.$OBJ);
done
ranlib $LIBA
