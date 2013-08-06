-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scobind
#pile

-- Tests scoping within applications

#include "axllib.as"

-- following have no special application symbol tables
local g0: () -> Integer
local g1: Integer -> Integer
local g2: (Integer,Integer) -> Integer

g1(a : Integer := 4)


-- following do have special application symbol tables
local f1: (n : Integer == 5) -> GaloisField(n)
local f2: (n : Integer, m: Integer) -> RectangularMatrix(n,m,Integer)

h1(aaa == 3)
Record(s : Integer, t : SingleFloat)

-- 'b' will be in outer scope, 'c' will be in apply scope
h2(b := 4, c == 5)
