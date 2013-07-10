--> testcomp
--> testerrs
#pile

#include "aldor"

local z: Boolean                     -- zb
local z: AldorInteger                     -- zi

z: Boolean == false
z: AldorInteger == 0

local left: (AldorInteger, Boolean) -> AldorInteger   -- lib-i
local left: (Boolean, Boolean) -> Boolean   -- lbb-b
local left: (Boolean, AldorInteger) -> Boolean   -- lbi-b

left(i: AldorInteger, b: Boolean): AldorInteger == i
left(b1:Boolean, b2:Boolean): Boolean == b1
left(b: Boolean, i: AldorInteger): Boolean == b


local f: (AldorInteger,Boolean) -> AldorInteger

f(i: AldorInteger, b: Boolean): AldorInteger ==
        b := left(z, b)                 -- lib-b+zi or lbb-b+zb: choose b
        i := left(z, b)                 -- same but choose i
        b := left(left(z, b), b)        -- lib-b+zi or lbb-b+zb: choose b
        i := left(left(z, b), b)        -- same but choose i

local g: (AldorInteger,Boolean) -> AldorInteger

g(i: AldorInteger, b: Boolean): AldorInteger ==
        b := left(b, z)                 -- lbi-b+zi or lbb-b+zb: ambiguous
        b := left(i, i)                 -- no meaning
        i

