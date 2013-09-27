TARGET	= aldor
SHELL	= bash

include ../../../net/web/generator/generator.mk

post-build:
	sed -i -e 's|"/home/contact"|"/contact"|g' $(HTML)
	sed -i -e 's|"/home/favicon.ico"|"/favicon.ico"|g' $(HTML)
	for i in aldor/*; do	\
	  rm -rf $${i/aldor\//};\
	done
	mv aldor/* .
	rmdir aldor
