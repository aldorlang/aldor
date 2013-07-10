--> testrun -fx -laxllib
--@ Bug Number:  bug1025.as 
--@ Fixed  by:  PAB   
--@ Tested by:  defarg9.as 
--@ Summary:    Default parameters caused mild confusion in type of a lambda 

-- Command line: axiomxl -Fx defparam.as
-- Version: 1.1.3
-- Original bug file name: defparam.as

----------------------------- defparam.as ----------------------------------
--
-- Looks like default parameters are still broken in 1.1.3:
--
-- % axiomxl -Fx defparam.as
-- % defparam
-- Looking in Foo(Integer, ??) for - with code 318693034
-- Export not found
--

#include "axllib.as"

macro Z == Integer;

Foo(R:Ring, avar:String == "x"):Ring == add {
	macro Rep == R;

	import from Z, Rep;

	0:% == foo 0;
	1:% == foo 1;
	(port:TextWriter) << (p:%):TextWriter   == port;
	coerce(n:SingleInteger):% == n::Z::%;
	coerce(n:Z):% == foo(n::R);
	(p:%)^(n:Z):% == p;
	foo(c:R):% == per c;
	-(p:%):% == p;
	(x:%) = (y:%):Boolean == false;
	(p:%) + (q:%):% == p;
	(p:%) * (q:%):% == p;
}

macro F == Foo(Z, "x");

bar():Boolean == {
	import from Z, F;
	a:F := 1;
	a - 1 = 0
}

bar();

