-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

local z: Boolean                     -- zb
local z: Integer                     -- zi

z: Boolean == false
z: Integer == 0

local left: (Integer, Boolean) -> Integer   -- lib-i
local left: (Boolean, Boolean) -> Boolean   -- lbb-b
local left: (Boolean, Integer) -> Boolean   -- lbi-b

left(i: Integer, b: Boolean): Integer == i
left(b1:Boolean, b2:Boolean): Boolean == b1
left(b: Boolean, i: Integer): Boolean == b


local f: (Integer,Boolean) -> Integer

f(i: Integer, b: Boolean): Integer ==
        b := left(z, b)                 -- lib-b+zi or lbb-b+zb: choose b
        i := left(z, b)                 -- same but choose i
        b := left(left(z, b), b)        -- lib-b+zi or lbb-b+zb: choose b
        i := left(left(z, b), b)        -- same but choose i

#if TestErrorsToo

local g: (Integer,Boolean) -> Integer

g(i: Integer, b: Boolean): Integer ==
        b := left(b, z)                 -- lbi-b+zi or lbb-b+zb: ambiguous
        b := left(i, i)                 -- no meaning
        i

#endif
