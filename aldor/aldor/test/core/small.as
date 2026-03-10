#include "foamlib"
#include "assertlib"
#pile

Foo == Cross(T: Order, T)
import from Assert SingleInteger
import from Assert Boolean

maximum(T: Order, tl: List T): T ==
    m: T := first tl
    for x in tl repeat
        if x > m then m := x
    return m

import from Boolean
import from SingleInteger
import from List Boolean, List SingleInteger

assertEquals(true, maximum(Boolean, [true, false]))
assertEquals(5, maximum(SingleInteger, [1,2,5,3,4]))
