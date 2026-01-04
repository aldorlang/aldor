#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

import from SInt

Foo(T: Type): with
    generator: % -> Generator T
    bracket: T -> %
    map: (%, T->T) -> %
    value: % -> T
    one: T -> %
== add
    Rep == T

    generator(x: %): Generator T == generate
        yield rep x

    bracket(t: T): % == per t

    bracket(g: Generator T): % ==
        for t in g repeat
            return per t
        never

    map(x: %, f: T -> T): % == bracket(f(t) for t in generator x)

    value(x: %): T == rep x
    one(t: T): % == per t


import from Foo SInt
foo(n: SInt): SInt == value(map(one n, prev))

--import from Assert Foo SInt
--import from Assert SInt
import from Foo SInt

--assertEquals(3, value(map(one 4, prev)))
