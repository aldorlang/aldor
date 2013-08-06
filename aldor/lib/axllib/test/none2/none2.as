-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib"

Int ==> SingleInteger;
import from Int;

f() : () == {
	print << 6 << newline;
}

g() : () == {
	print << 7 << newline;
}

g(f())
