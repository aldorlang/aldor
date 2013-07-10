#!/bin/bash
    
OKFILES="build.sh Makefile lno testaldor unitools"

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done
fi

for dir in lno testaldor unitools ; do
	(cd $dir ; bash build.sh $*)
done
