-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs

-- This file tests using ':*' to avoid having
-- a type imported.

#include "axllib.as"
#pile

local x
x :* Integer := 1$Integer

#if TestErrorsToo

-- In the following, the first + should have no meanings,
-- and the second should compile without complaint.

x + x +$Integer x

#endif
