#!/bin/sh
##########################################################
#                                                        #
# Creates libaldor.a given libaldor.al (release version) #
#                                                        #
# The Aldor Development Group (2001-2002)                #
#                                                        #
##########################################################

# Check usage
if [ $# != 2 ]
then
    echo "Usage: $0 \$ALDORLIBROOT \$LIBAL"
    exit 1
fi

# Variables
LIBEXT=lib
ALDORLIBROOT=$1
LIBDIR=$ALDORLIBROOT/lib
LIBAL=$LIBDIR/$2
if [ "$MACHINE" = "win32msvc" ]; then
	ALDOR=aldor.sh
	UNICL=unicl.sh
	LIBEXT="lib"
	OBJ="obj"
	LIBA=$LIBDIR/`basename $LIBAL .al`.$LIBEXT
	#AR="lib"
	#ARFLAGS="/nologo `cygpath -m $LIBA`"
	#ARFLAGSFIRST="/nologo /OUT:`cygpath -m $LIBA`"
	AR="ar"
	ARFLAGS="rv $LIBA"
elif [ "$MACHINE" = "win32gcc" ]; then
	ALDOR=aldor.sh
	UNICL=unicl.sh
	LIBEXT="a"
	OBJ="o"
	LIBA=$LIBDIR/`basename $LIBAL .al`.$LIBEXT
	AR="ar"
	ARFLAGS="rv $LIBA"
else
	ALDOR=aldor
	UNICL=unicl
	LIBEXT="a"
	OBJ="o"
	LIBA=$LIBDIR/`basename $LIBAL .al`.$LIBEXT
	AR="ar"
	ARFLAGS="rv $LIBA"
fi

# Remove alias 'rm' (-a removes ALL aliases for rm, thus avoiding msg if not aliased)
unalias -a rm

# compile sal_util.c
cd src/util
$UNICL -c -O sal_util.c

#if [ "$MACHINE" = "win32msvc" ]; then
#	if [ -e $LIBA ]; then 
#		$AR $ARFLAGS sal_util.$OBJ
#	else
#		$AR $ARFLAGSFIRST sal_util.$OBJ
#	fi
#else
	$AR $ARFLAGS sal_util.$OBJ
#fi

rm -f sal_util.$OBJ
cd ../..

# compile libaldor.al into libaldor.a
for i in `ar t $LIBAL`; do (
	ar x $LIBAL $i;
	$ALDOR -fo -q5 -qinline-all -csmax=0 $i;
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

