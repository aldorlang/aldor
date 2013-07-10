#!/bin/bash

# Recompile some source files and rebuild the library
if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi

CC=gcc
CFLAGS="-g -O3  -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"

AR=ar
ARFLAGS=r
if [ `uname -o` = "Cygwin" ] ; then EXE=exe ; else EXE=out ; fi

function buildlib () {
    Lib=$1; shift

    cp $ALDORROOT/lib/$Lib .

    for SrcFile in $* ; do
	echo "Compiling $SrcFile"
	ObjFile=`echo $SrcFile | sed -e 's/\\.[cs]/.o/'`
	$CC $CFLAGS -c $SrcFile
	$AR $ARFLAGS $Lib $ObjFile
	rm $ObjFile
    done
    ranlib $Lib
    mv $Lib $ALDORROOT/lib
}

# Libraries
echo "--- library"
buildlib libascomp.a $*

# Main programs
echo "--- aldor_t"
$CC $CFLAGS main_t.c -L$ALDORROOT/lib -lascomp -lm
mv a.$EXE $ALDORROOT/bin/aldor_t

echo "--- aldor"
$CC $CFLAGS main.c   -L$ALDORROOT/lib -lascomp -lm
mv a.$EXE $ALDORROOT/bin/aldor
