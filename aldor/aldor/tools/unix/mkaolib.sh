#!/bin/bash
echo Args: $*
set -e

file=$1
shift
if [[ $1 =~ .al$ ]];
then
    cp $1 $file
    shift
fi


for i in $*;
do
    ar cr $file $i
done
