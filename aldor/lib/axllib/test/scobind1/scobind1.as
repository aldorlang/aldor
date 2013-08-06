-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax -Mno-ALDOR_W_CantUseArchive
--> testrun -l axllib
#pile

-- This file tests for situations when one name covers another.

#include "axllib.as"

import from SingleInteger

#if TestErrorsToo

local y : SingleInteger := 1

f(x: SingleInteger, w : SingleInteger):SingleInteger ==
    local w                             -- bad, parameters cannot be declared free or local
    print<<"Parameter x = "<<x<<newline
    print<<"Inner x = "<<x<<newline where
          local x : SingleInteger == 3  -- bad, local const with same name as parameter
          local y : SingleInteger == 4  -- bad, local const with same name as outer variable
    x

local
    q : SingleInteger := 7
    r : SingleInteger == 7

q + r where
    q : SingleInteger := 700            -- give implicit local message
    r : SingleInteger := 700            -- give implicit local message

g0(q : SingleInteger): SingleInteger ==
    print<<"Inner lexical variable q = "<<q<<newline where
        q : SingleInteger := 700        -- give implicit local message
    print<<"Parameter q = "<<q<<newline
    q

-- following tests that one cannot assign to a library or archive

AxlLib := Array

#library AxlDem "axldem"
AxlDem == Array

#endif

local
    z : SingleInteger := 2
    u : SingleInteger == 2

print<<"Outer lexical variable z = "<<z<<newline
print<<"Inner lexical variable z = "<<z<<newline where
    local z : SingleInteger
    z := 200                            -- ok, this covers outer local

print<<"Outer lexical constant u = "<<u<<newline
print<<"Inner lexical variable u = "<<u<<newline where
    local u : SingleInteger
    u := 200                            -- ok, this covers outer local

g(q : SingleInteger): SingleInteger ==
    print<<"Inner lexical variable q = "<<q<<newline where
        local q : SingleInteger
        q := 700
    print<<"Parameter q = "<<q<<newline
    q

g(7)

f2(z: SingleInteger): SingleInteger ==  -- parameter covers outer local
        print << "[Outer z = " << z << "] "

        g2(z: SingleInteger): SingleInteger ==  -- parameter covers outer parameter
                print << "[Inner z = " << z << "] "<<newline
                z

        g2(z*z + 1)

f2(100)
f2(10000)

f3(z: SingleInteger, u: SingleInteger): SingleInteger == -- parameters cover outer locals
        print<<"Inner z + u = "<<(w := z + u)<<newline
        w


print<<"Outer z + u = "<<z+u<<newline
f3(200,300)
