TARGET	= aldor
SHELL	= bash

include ../../../net/web/generator/generator.mk

post-build:
	for i in aldor/*; do	\
	  rm -rf $${i/aldor\//};\
	done
	mv aldor/* .
	rmdir aldor
