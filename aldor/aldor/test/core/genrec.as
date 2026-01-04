#include "foamlib"
#include "assertlib"
#pile
#library X "x.ao"
import from X

SInt ==> SingleInteger

R == Record(v: SInt)
import from R
foo(n: SInt): XGenerator R == xgenerate
    for x in 1..n repeat yield [x]

bar(n: SInt): SInt ==
    acc: SInt := 0
    xfor r in foo n repeat
        acc := acc + r.v
    return acc

import from Assert SInt
assertEquals(6, bar 3)
