#!/bin/sh
# interactive session example

aldor -Gloop -Mno-release <<ThatsAll
#int timing off
#include "axllib"

SI ==> SingleInteger
I ==> Integer
F ==> SingleFloat
DF ==> DoubleFloat

-- This is a comment

x : List String := ["natascia", "I", "love", "you"]

c : SI := 10
d : SI := 7

member?("you", x)
if member?("natascia",x) then c else d

import from List String

if member?("no member",x) then c else d

first x
first rest x

#int
#int help
#int msg-limit 20
Integer
#int msg-limit 0
Integer

a : Array SI := [1,2,3,4]

for h in a repeat { print << h << newline; }
#quit
ThatsAll
