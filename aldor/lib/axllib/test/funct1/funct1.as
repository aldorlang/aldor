-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile


y: DoubleFloat == 0.0
y: SingleFloat == 2.2

b: Boolean  == false
b: Integer == 2

left(i: Integer , q: Integer,  b: Boolean,      s: Boolean): Integer == i
left(a: Integer,  q: Boolean,  b: Boolean,      s: Boolean): Boolean == b
left(b: Integer,  q: Boolean,  i: Integer,      s: Boolean): Boolean  == q

#if TestErrorsToo

f1 () : Integer == left(b, false, y, false)     -- 0 poss. types
f2 () : Boolean  == left(b, false, y, false)    -- 0 poss. types


foo() : Boolean == left( 1, false, b, false )   -- 2 poss. meanings 

#endif
