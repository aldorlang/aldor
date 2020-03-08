#!/bin/sh

echo "current directory is: " $(pwd)
echo files:
ls
root=`pwd`
mkdir build
cd aldor
./autogen.sh

cd ../build
../aldor/configure --prefix=$root/opt
make V=1
