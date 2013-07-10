#!/bin/bash
    
IncSrcs="foamlib.as"
AXL0Srcs="
	lang.as machine.as basic.as foamcat.as tuple.as gener.as 
	boolean.as segment.as sinteger.as sfloat.as pointer.as char.as 
	parray.as array.as list.as langx.as string.as
"
AXL1Srcs="
	format.as partial.as oslow.as fname.as file.as opsys.as textwrit.as
"
OKFILES="
	README Makefile build.sh 
	$IncSrcs $AXL0Srcs $AXL1Srcs
"
SAMPLEDIR=$ALDORROOT/share/samples/libfoamlib

if [ "$1"x = "junk"x ] ; then
	for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
	echo ">>> Enter `pwd`"

	if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi

	export PATH=$ALDORROOT/bin:$PATH

	SRC=../../src

	ALDOR=$ALDORROOT/bin/aldor
	ALDORFLAGS="-Zdb -Wcheck -Q3 -Qinline-all" 
			# -Wcheck -Cargs=-g -Qno-cc -Ffm -Wgc
	AR=ar
	ARFLAGS=r
	function donaked () {
		echo "Compiling $*"
		fbase=$1; shift
		$ALDOR -Fao -Fo $ALDORFLAGS $* $fbase.as 
		$AR $ARFLAGS libfoamlib.al $fbase.ao
		$AR $ARFLAGS libfoamlib.a  $fbase.o
		rm $fbase.ao $fbase.o
	}
	function doaldor() { donaked $* -l foamlib ; }

	# Ensure directories exist
	mkdir -p $ALDORROOT/include
	mkdir -p $ALDORROOT/lib
	mkdir -p $ALDORROOT/bin
	mkdir -p $SAMPLEDIR

	# Include dir
	cp  foamlib.as	   $ALDORROOT/include
	cp $SRC/foam_c.h   $ALDORROOT/include # shd clean so no other .h needed
	cp $SRC/optcfg.h   $ALDORROOT/include
	cp $SRC/foamopt.h  $ALDORROOT/include
	cp $SRC/cconfig.h  $ALDORROOT/include
	cp $SRC/platform.h $ALDORROOT/include


	# Start library
	rm -f libfoamlib.a libfoamlib.al

	# AXL0Srcs
	donaked lang -M no-ALDOR_W_WillObsolete
	doaldor machine 
	doaldor basic   -Q inline-limit:18
	doaldor foamcat 
	doaldor tuple
	doaldor gener
	doaldor boolean
	doaldor segment
	doaldor sinteger
	doaldor sfloat
	doaldor pointer
	doaldor char
	doaldor parray
	doaldor array
	doaldor list
	doaldor langx
	doaldor string

	# AXL1Srcs
	doaldor format
	doaldor partial
	doaldor oslow
	doaldor fname
	doaldor file
	doaldor opsys
	doaldor textwrit

	# Finish library
	ranlib libfoamlib.a
	mv libfoamlib.a libfoamlib.al $ALDORROOT/lib

	# Samples
	echo "Copying samples..."
	cp 	$IncSrcs $AXL0Srcs $AXL1Srcs $SAMPLEDIR

	echo "<<< Exit `pwd`"
fi
