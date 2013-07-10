-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"

-- 2 meanings for y

y: Integer == 1;
y: SingleInteger == 0;

t := k;

x := y;   -- lhs undeclared

z: Boolean := y


