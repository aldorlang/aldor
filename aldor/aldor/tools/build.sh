#!/bin/bash
OKFILES="build.sh Makefile unix"

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done
fi
(cd unix ; bash build.sh $*)
