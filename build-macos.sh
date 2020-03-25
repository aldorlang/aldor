#!/bin/sh

root=`pwd`
mkdir build
cd aldor
./autogen.sh

cd ../build
../aldor/configure --prefix=$root/opt
make V=1
