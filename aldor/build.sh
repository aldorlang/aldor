#!/bin/sh
###### 2013 -- This builds a system on Linux.  The old make files are a maze.

#export ALDORROOT=/tmp/ar1
#mkdir  $ALDORROOT

export MACHINE=linux
export ALGEBRAROOT=$ALDORROOT

export PATH=$ALDORROOT/bin:$PATH
(cd aldor ; bash build.sh build)

(cd lib/aldor ; make library )
(cd lib/algebra ; cp include/* $ALDORROOT/include ; cd src ; make release )
