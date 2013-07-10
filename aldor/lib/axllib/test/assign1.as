-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"

-- 2 meanings for y

y: Integer == 1;
y: SingleInteger == 0;

(x1:Integer,u1:Integer) := (y,y);  -- Ok

#if TestErrorsToo

(x2:Integer,u2) := (y,y); 

(x3,u3) := (y,y);

(x4:Boolean,u4) := (y,y);

for x in l repeat print << x;  --err msg only for l, that is the real problem

#endif
