-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--- simple fluid tests

--> testcomp
--> testrun -l axllib
#pile

-- fluid restrictions: 
--   fluids must be consistently typed throughout a prog; 
--    all fluids called 'x' are the same variable.
--   a fluid must be assigned in a fluid stmt. before it is used,
--   or assigned to anywhere else.
--   fluids and locals and frees don't mix, and the error messages are 
--   misleading.

#include "axllib.as"

#assert true
{
#if true
T1(): () == {
	import from SingleInteger;
	fluid x: SingleInteger := 1;
	sub1(): () == { print << x<<newline; };
	sub2(): () == { fluid y: SingleInteger; print <<y<<newline; };

	sub3(): () == { fluid y:= 2; sub2(); sub1(); sub4() };
	sub4(): () == { fluid x:=3; fluid y:=4; sub1(); sub2(); };

	sub3();
}

T1();

#endif

#if true
--- Multiple value fluids
T2(): () == {
	fluid x: SingleInteger:=0;
	fluid y: String := "";
	sub1(): (SingleInteger, String) == { return (3, "hello")}
	(x, y) := sub1();
	print << x<<" "<<y<<newline;
	}
T2();

#endif
}
