#include "algebra"
#pile

import from TextWriter
import from Character
import from String

TestComplex: TestCategory with == add
  test(): () == 
    import from Complex Integer
    import from Assert Complex Integer
    import from Partial Complex Integer
    import from Assert Integer
    import from Integer

    minusOne: Complex Integer := -1
    sqrtMinusOne := complex(0, 1)
    assertEquals(minusOne, sqrtMinusOne * sqrtMinusOne)
    --assertEquals(retract reciprocal(minusOne), -1)
    for i in 1..10 repeat
    	a: Complex Integer := random()
	b: Complex Integer := random()
	c: Complex Integer := random()
        assertEquals(a, a)
        assertEquals(0, a + (-a))
        assertEquals("comm", a + b, b+a)
	assertEquals("-1", -a, (-1)@Complex(Integer) * a)
	assertEquals("anticomm", a - b, -(b - a))
	assertEquals("dist", a * c + b * c, (a+b) * c)

import from TestComplex;
test()

