#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

#library X "x.ao"
import from X
import from SInt

Foo(T: Type): with
    xgenerator: % -> XGenerator T
    xbracket: T -> %
    map: (%, T->T) -> %
    value: % -> T
    one: T -> %
== add
    Rep == T

    xgenerator(x: %): XGenerator T == xgenerate
        yield rep x

    xbracket(t: T): % == per t

    xbracket(g: XGenerator T): % ==
        xfor t in g repeat
            return per t
        never

    map(x: %, f: T -> T): % == xbracket(f(t) xfor t in xgenerator x)

    value(x: %): T == rep x
    one(t: T): % == per t


import from Foo SInt
foo(n: SInt): SInt == value(map(one n, prev))

import from Assert Foo SInt
import from Assert SInt
import from Foo SInt

assertEquals(3, value(map(one 4, prev)))
