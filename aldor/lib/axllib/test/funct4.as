-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

y: DoubleFloat == 0.0
y: SingleFloat == 2.2

b: Boolean  == false
b: Integer == 2

left(i: Integer , q: Integer,  b: Boolean,  s: Boolean): Integer == i
left(b: Integer,  q: Boolean,  i: SingleFloat,  s: Byte): Boolean == q
left(a: Integer,  q: Boolean,  b: Boolean,  s: Boolean): Boolean == b
left(b: Integer,  q: Boolean,  i: Integer,  s: Boolean): Boolean == q
left(a: Integer,  q: Byte,     i: Byte,  s: Boolean): Boolean == s
left(a: Integer,  q: Byte ): Integer == a


--#if TestErrorsToo

f1 () : Integer == left(b, false, y, false) -- 0 poss. types
f2 () : Boolean  == left(b, false, y, false)    -- 0 poss. types
f3 () : Boolean  == left( b ) -- par. 2 doesn't match

--#endif
