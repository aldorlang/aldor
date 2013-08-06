-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

local v, w, x, z: Integer
local u, j: Integer -> Integer -> Integer
local T: Type

-- These should generate warnings.
j(x)
  z
  w
u(v)
  w
a1: T
v = w

-- These should NOT generate warnings.
j(x) .
  z
  w
u(v) .
  w
do a2: T
do w = w
