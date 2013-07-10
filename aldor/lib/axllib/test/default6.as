-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib.as"

Bit ==> Boolean

y: DoubleFloat == 0.0
y: SingleFloat == 2.2

b: Bit == false
b: Integer == 2

foo( a: Integer, c: Integer == 1, d: DoubleFloat) : Integer == c

#if TestErrorsToo

a1: Integer == foo(1)
a2: Integer == foo(2,b)
a3: Integer == foo(false, 1,1)
a4: Integer == foo()

#endif

