-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib.as"

import from Integer

g: Record( a:Integer , b:Integer) := [1,2]
h: Record( p:Integer , q:Integer) := [1,2]
g.a := 1
x := a
g.x := 1

#if TestErrorsToo

y := ff

g.y := 1  -- .$ := undef
y.x := 1  -- $. := undef

h.a := 1  -- $. def, .$ def $.$ undef
g.a := false  -- type mismatch lhs,rhs

a.a := 1  -- $ def $. no sense
g.a := y  -- := $ undef

g(a,a) := 1  -- too many args for set!

uuu: Boolean == (g.a := 1)
#endif

