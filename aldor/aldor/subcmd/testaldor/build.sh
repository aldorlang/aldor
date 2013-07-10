#!/bin/bash
    
OKFILES="
	Makefile build.sh
	testaldor.c testaldor.h
	stat.h0 tx_opsys.c tx_unix.c tx_win32.c
"
if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
    echo ">>> Enter `pwd`"

    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi
    
    CC=gcc
    CFLAGS="-I../../src -O"
    
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
    
    installc toolbin testaldor # Run compiler test suite.

    echo "<<< Exit `pwd`"
fi

