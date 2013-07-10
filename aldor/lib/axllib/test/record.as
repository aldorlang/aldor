-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

g: Record( e:Record( a:Integer )) := [[1]]
a: Record( x:Integer, y:Integer ) := [ 1, 2 ]

g.e.a := 1

f():Record( t:Integer) == [10]

#if TestErrorsToo
k := x

a.13 := 3
f().1 := 1
f().t := 2
a.w := 3

print a.c

g.e.y := 2
g.e.e := 1
g.z.a := 1

#endif
