#!/bin/bash
    
Sources="runtime.as foam_l.lsp"
ReferenceFiles="hashax.boot"
SchemeFiles="axllib.scm foam_s.scm"

OKFILES="
	Makefile build.sh
	$Sources $ReferenceFiles $SchemeFiles
"

SRC=../../src
export PATH=$ALDORROOT/bin:$PATH

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
    echo ">>> Enter `pwd`"

    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi
    
    CC=gcc
    CFLAGS="-I$SRC -DFOAM_RTS -O -DNDEBUG"

    AR=ar
    ARFLAGS=r
    
    ALDOR=aldor

    # !! Put any_as.s on sparc
    LIBFOAMC="
	$SRC/stdc.c $SRC/cport.c $SRC/opsys.c $SRC/btree.c $SRC/store.c
	$SRC/memclim.c $SRC/bigint.c $SRC/dword.c $SRC/util.c $SRC/table.c
	$SRC/xfloat.c $SRC/foam_c.c $SRC/foam_cfp.c $SRC/foam_i.c
	$SRC/output.c $SRC/foamopt.c $SRC/compopt.c $SRC/timer.c
    "

    # Install include files
    cp $SRC/foam_c.h   $ALDORROOT/include  # should clean so no others needed
    cp $SRC/optcfg.h   $ALDORROOT/include
    cp $SRC/foamopt.h  $ALDORROOT/include
    cp $SRC/cconfig.h  $ALDORROOT/include
    cp $SRC/platform.h $ALDORROOT/include
    
    # Install run time for lisp
    cp foam_l.lsp  $ALDORROOT/lib

    # Build library
    rm -f libfoam.a

    for SrcFile in $LIBFOAMC ; do
	ObjFile=`echo $SrcFile | sed -e 's/\\.[cs]/.o/'`
        ObjFile=`basename $ObjFile`
	echo "Compiling $SrcFile"
	$CC $CFLAGS -c $SrcFile
	$AR $ARFLAGS libfoam.a $ObjFile
	rm $ObjFile
    done

    ranlib libfoam.a
    mv libfoam.a $ALDORROOT/lib

    # Add the domain representation written in Aldor to the runtime
    BuldRuntimeFlags="
    	-Q3 -Qinline-all -Qno-cc
    	-Fao -Fo -Flsp -Fc
    	-W runtime -W check -M no-ALDOR_W_OverRideLibraryFile
    	-Csmax=0 -Cargs=-g
    "
    echo "Compiling runtime: $ALDOR $BuldRuntimeFlags runtime.as"
    $ALDOR $BuldRuntimeFlags runtime.as
    
    # Check there are no "gets".
    grep 'fiFileInitializer(' runtime.c | \
     sed -e 's/^[^"]*"\([^"]*\).*$$/*** Error: gets not allowed from \1./' | \
     grep -v rtexns
    rm runtime.c
    
    $AR $ARFLAGS $ALDORROOT/lib/libfoam.al runtime.ao
    $AR $ARFLAGS $ALDORROOT/lib/libfoam.a  runtime.o
    ranlib $ALDORROOT/lib/libfoam.a
    rm runtime.ao runtime.o
    mv runtime.lsp $ALDORROOT/lib

    echo "<<< Exit `pwd`"
fi
