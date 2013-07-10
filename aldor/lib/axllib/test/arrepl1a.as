-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib"

Int ==> SingleInteger;

#if AddExport
P: with {
	z: Int;
}
== add {
	z: Int == 9;
}
#endif

A: with {
	x: Int;
	y: Int;
}
== add {
	x: Int == 11;
	y: Int == 8;
}

#if AddExport
Q: with {
	z: Int;
}
== add {
	z: Int == 9;
}
#endif
