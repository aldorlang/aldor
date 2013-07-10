#!/bin/sh

(aldor -G loop -Mno-release -l axllib | grep -v "GC:") <<TheEnd
#int timing off
#include "axllib"
import from Integer
i := 3 + 3
j := gcd
j(i,i)
TheEnd
