#!/bin/bash
    
OKFILES="
	Makefile build.sh
	tools subcmd src lib contrib

	Authors Current Ports Timings ToDo TsCheck TsFrozen Version Win32Port
"

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done
fi

(cd tools;   bash build.sh $*)
(cd subcmd;  bash build.sh $*)
(cd src;     bash build.sh $*)
(cd lib;     bash build.sh $*)
(cd contrib; bash build.sh $*)

