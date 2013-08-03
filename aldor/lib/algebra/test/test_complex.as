#include "algebra.as"
#pile
test(): () == 
    import from Complex Integer
    import from Assert Complex Integer

    assertEquals(minusOne, i * i)
    for i in 1..10 repeat
    	a := random()
	b := random()
        assertEquals(a + b, b+a)
	assertEquals(-a, -1 * a)
	assertEquals(a - b, -(b - a))

	