#include "algebra"
#pile

import from TextWriter
import from Character
import from String

TestMachineInteger: TestCategory with == add
	import from TextWriter
	import from Character
	import from String

	test(): () == 
		import from Assert MachineInteger
    		import from MachineInteger
		import from GeneralAssert

    		assertTrue(zero? 0)
		assertFalse(zero? 1)
    
		assertFalse(min-1 < 0)
		assertFalse(max + 1 > 0)
		
		assertTrue(testSerial(0))
		assertTrue(testSerial(1))
		assertTrue(testSerial(max))
		assertTrue(testSerial(-1))
		assertTrue(testSerial(min))
		for i in 1..100 repeat
    			a: MachineInteger := random()
			assertTrue(testSerial(a))
			

        testSerial(n: MachineInteger): Boolean ==
		buf: StringBuffer := new();
		coerce(buf) << n
		stdout << "conv " << n << " " << string(buf) << newline
		m: MachineInteger := << coerce(buf)
		stdout << "blah " << n << " " << string(buf) << " " << m << newline
		m = n

	string(n: MachineInteger): String == 
		buf: StringBuffer := new();
		coerce(buf) << n
	   	string(buf)

import from TestMachineInteger
test()

