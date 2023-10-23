#include "foamlib"
#pile

Foo == Cross(T: Order, T)

maximum(T: Order, tl: List T): T ==
    m: T := first tl
    for x in tl repeat
        if x > m then m := x
    return m

import from Boolean
import from SingleInteger
import from List Boolean, List SingleInteger

stdout << maximum(Boolean, [true, false]) << newline
stdout << maximum(SingleInteger, [-1, -2, -3]) << newline
