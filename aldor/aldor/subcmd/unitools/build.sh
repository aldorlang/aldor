#!/bin/bash
    
OKFILES="
	Makefile build.sh
	unicl.c unicl.h platform.c
"
SRC=../../src

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
    echo ">>> Enter `pwd`"

    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi
    
    CC=gcc
    CFLAGS="-I$SRC -O"
    
    function announce () {
    	echo Building $1
    }
    function install () {
    	announce $2
    	mkdir -p $ALDORROOT/$1
    	cp    $2 $ALDORROOT/$1/$2
    	chmod 755 $ALDORROOT/$1/$2
    }
    function installc () {
    	announce $2
    	mkdir -p $ALDORROOT/$1
        dir=$1 ;    shift
    	target=$1 ; shift
    	$CC $CFLAGS $target.c $* -o $ALDORROOT/$dir/$target
    	chmod 755 $ALDORROOT/$dir/$target
    }
    
    # Tools delivered to end-user [bin directory]

    NEEDC="
	$SRC/file.c $SRC/fname.c $SRC/opsys.c $SRC/store.c $SRC/memclim.c
	$SRC/fluid.c $SRC/btree.c $SRC/stdc.c $SRC/strops.c
	$SRC/debug.c $SRC/xfloat.c $SRC/buffer.c $SRC/timer.c
	$SRC/util.c $SRC/list.c $SRC/format.c $SRC/cfgfile.c
    "
    
    installc bin unicl $NEEDC 
    (cd $ALDORROOT/bin ; rm -f uniar; ln -s `which ar` uniar)
    installc bin platform

    echo "<<< Exit `pwd`"
fi
