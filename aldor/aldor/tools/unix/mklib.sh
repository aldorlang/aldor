#!/bin/sh
# Usage: makeobject sys libname
echo $ALDORROOT
sys=$1
libname=$2
alfile=$ALDORROOT/lib/$libname.al
afile=$libname.a

if [ ! -f $alfile ] ;
then
	alfile = $ALDORROOT/share/lib/$libname.al
fi
if [ ! -f $alfile ] ;
then
	echo "Could not find $libname.al"
	exit 1
fi

echo "Rebuilding $afile for $sys"
for x in `ar t $alfile`;
do
	echo $x
	uniar x $alfile $x
	aldor -Csys=$sys -csmax=0 -fo -O $x
	ofile=`basename $x .ao`.o
	if [ ! -f $ofile ] ; then echo "Compile failed. exiting." ; exit 1 ; fi
	ar r $afile $ofile
	rm $x $ofile
done
