-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs

#include "axllib.as"
#pile

x: Integer := 1

-- This example is fine for two reasons.
if x < 0 then x else x

-- This example is fine since the value is not needed.
if x < 0 then x

-- This example is fine since both alternatives are present.
x := (if x < 0 then x else x)

-- This example is fine since the alternatives can be different if not needed.
if x < 0 then 2 else "hello"

#if TestErrorsToo

-- A value is needed but 'if' expression has no 'else'.
x := (if x < 0 then x)

-- Cannot unify the two branches.
u := if x < 0 then 2 else "hello"
#endif
