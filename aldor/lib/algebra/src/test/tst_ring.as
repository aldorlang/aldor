#include "algebra"
#pile

import from String

TestRing(R: Ring): with TestCategory == add
    test(): () ==
        import from Integer
        import from Assert R
        for i in 1..10 repeat
	    a: R := random()
	    b: R := random()
	    c: R := random()
            assertEquals(a, a)
            assertEquals(0, a + (-a))
            assertEquals("comm", a + b, b+a)
	    assertEquals("-1", -a, (-1)@R * a)
	    assertEquals("anticomm", a - b, -(b - a))
	    assertEquals("dist", a * c + b * c, (a+b) * c)
