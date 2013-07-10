#!/bin/sh
# Error recovery in interactive mode

aldor -Gloop -Mno-release <<ThatsAll
--int verbose on
#int timing off
#include "axllib.as"
I ==> Integer
SI ==> SingleInteger
ST ==> String
BO ==> Boolean
F ==> SingleFloat

x : I := 10
x : SI := 20
x
x : I := 1
x : ST := "tata"
x : I := "fads"
x

foo(x:BO):BO == 2
foo(x:BO):BO == not x
foo true

import from SI
foo(x:SI):SI == x
foo 32
foo false

foo(x:BO):BO == x
y
foo false

k : F == false
h := 1.0  	-- bug
k : F := false
h := 1.0
k : F := 3.0
h := 1.0
k

gg(x:I):I == x
gg 10
gg(x:I):I == false
y
gg 10
#quit
ThatsAll
