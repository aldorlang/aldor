#!/bin/bash
    
OKFILES="build.sh Makefile libfoamlib libfoam"

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done
fi

(cd libfoamlib;  bash build.sh $*)
(cd libfoam;     bash build.sh $*)
