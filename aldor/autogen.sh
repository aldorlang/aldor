#!/bin/sh

echo " + libtoolize"	&& libtoolize
echo " + aclocal"	&& aclocal
echo " + automake"	&& automake --add-missing
echo " + autoconf"	&& autoconf
