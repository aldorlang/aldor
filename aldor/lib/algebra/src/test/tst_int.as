#include "algebra"
#include "aldorio"
#pile

TestInteger: TestCategory with == add
	import from Integer
	import from Assert Integer
	import from List Integer

        test(): () ==
	   assertTrue(1 > 0$Integer)
	   assertFalse(1 < 0$Integer)
	   test()$TestRing(Integer)
	   --assertFalse("1 is not prime", prime? 1)
	   assertTrue("3 is prime", prime? 3)
	   assertTrue("2 is prime", prime? 2)
	   for i in [2,3,5,7,11,13,17] repeat
	       assertTrue(prime? i)
	   for i in 1..1000 repeat
	       assertFalse(prime?((random() mod 1000) * (random() mod 1000)))

test()$TestInteger;