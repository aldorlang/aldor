-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"
#pile

@ Start
import from Boolean
import from Integer

@ Initialize
print<<"--++ Let's  Fib ++--"<<newline
(i, j) := (1, 1)
n := 0

@ Work
print<<n<<". i = "<<i<<", j = "<<j<<newline
(i, j) := (i+j, i)
n := n + 1
if n < 10 then goto Work

@ Done
print<<"--++ That's All ++--"<<newline

@ End
