-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axldem -l axllib
#pile
#include "axllib.as"
#library  DemoLib "axldem"

R ==> DoubleFloat
I ==> SingleInteger

import from DemoLib
import from DoubleFloat
import from SingleInteger
import from DoubleFloatElementaryFunctions

a      := 0.0
b      := 2.0
relerr := 1.0e-10
abserr := 0.0

fun(x: R): R == if x = 0.0 then 1.0 else sin(x)/x

print<<"Integrating sin(x)/x from "<<a<<" to "<<b<<newline

(result, errest, nofun, flag) := quanc8(fun, a, b, abserr, relerr)

print<<"Result = "<<result<<"   Error estimate = "<<errest<<newline
if flag ~= 0.0 then print<<"Warning..result may be unreliable.  flag = "<<flag<<newline
