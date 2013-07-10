-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

-- This example is fine for two reasons.
(x : Integer) := x := 1; ( x < 0 => x; x := 1 )

-- This example is fine since the value is not needed.
(x : Integer) := x := 1; ( x < 0 => x )

-- This example is fine since another statement follows the exit.
(x : Integer) := ( x := 1; x < 0 => x; x := 1 )

-- A value is needed but 'exit' may not produce one.
#if TestErrorsToo
(x : Integer) := ( x := 1; x < 0 => x )
#endif
