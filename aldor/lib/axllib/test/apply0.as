-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

y: DoubleFloat == 0.0
y: SingleFloat == 2.2

b: Boolean  == false
b: Integer == 2

apply(i: Integer , q: Integer,  b: Boolean,  s: Boolean): Integer == i
apply(b: Integer,  q: Boolean,  i: SingleFloat,  s: Byte): Boolean == q
apply(a: Integer,  q: Boolean,  b: Boolean,  s: Boolean): Boolean == b
apply(b: Integer,  q: Boolean,  i: Integer,  s: Boolean): Boolean == q
apply(b: Boolean,  q: Boolean,  i: DoubleFloat, s: Boolean):String == "Natascia"
apply(a: Integer,  q: Byte,     i: Byte,  s: Boolean): Boolean == s
apply(a: Integer,  q: Byte ): Integer == a



f1 () : Integer == apply(b, false, y, false) -- 0 poss. types
f2 () : Integer == 1(false, y, false)
f3 () : String  == apply(b, false, y, false) -- 0 poss. types
f4 () : Integer == false(false, 1.2, 1)
