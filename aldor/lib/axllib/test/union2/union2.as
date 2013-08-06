-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
--> testerrs

#include "axllib"

f(): () == {
	import from Union(x: Integer, y: Integer, z: String);
	print << ([x==2] case x)<<newline;
	print << ([x==2] case y)<<newline;
	print << [y==2] << newline;
	print << ([x==2] = [y==2]) << newline;
#if TestErrorsToo
	[3];
#endif
}

f();
