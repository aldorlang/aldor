#!/bin/sh
unicl -I/scl/people/jcai/aldor1.0.2/linux/1.0.2/include -c deb400.c
gcc  -g -fwritable-strings -w -ffloat-store -I /scl/people/jcai/aldor1.0.2/linux/1.0.2/include -c deb400.c
unicl -I/scl/people/jcai/aldor1.0.2/linux/1.0.2/include -c aldormain400.c
gcc  -g -fwritable-strings -w -ffloat-store -I /scl/people/jcai/aldor1.0.2/linux/1.0.2/include -c aldormain400.c
unicl deb400.o aldormain400.o -L. -L/scl/people/jcai/aldor1.0.2/linux/1.0.2/share/lib \
-L/scl/people/jcai/aldor1.0.2/linux/1.0.2/lib -o  deb400 -ldebuglib -laldor -lfoam
gcc  -g -L /usr/X11/lib -o deb400 aldormain400.o deb400.o -L . -L /scl/people/jcai/aldor1.0.2/linux/1.0.2/share/lib \
-L /scl/people/jcai/aldor1.0.2/linux/1.0.2/lib -ldebuglib -laldor -lfoam -lm

