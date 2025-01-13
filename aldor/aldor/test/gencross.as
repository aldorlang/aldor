#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

#library X "x.ao"
import from X
import from SInt

foo(n: SInt): Generator Cross(SInt, SInt) == generate
    yield (n, n+1)

foo2(n: SInt): Generator SInt == (i+j) for (i, j) in foo(n)
foo3(n: SInt): Generator Cross(SInt, SInt) == (i,j) for (i, j) in foo(n)

xfoo(n: SInt): XGenerator Cross(SInt, SInt) == xgenerate
    yield (n, n+1)

xfoo2(n: SInt): XGenerator SInt == (i+j) xfor (i, j) in xfoo(n)

xfoo3(n: SInt): XGenerator SInt == xgenerate
    xfor (k, l) in xfoo(n) repeat
        yield k+l

xfoo4(n: SInt): XGenerator Cross(SInt, SInt) == (i,j) xfor (i, j) in xfoo(n)

import from Assert SInt

xfor a in xfoo2(5) repeat assertEquals(11, a)

xfor (a, b) in xfoo4(12) repeat
    assertEquals(12, a)
    assertEquals(13, b)

