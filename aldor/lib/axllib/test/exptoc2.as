-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c

#include "axllib"

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
}

import {
	bar:		() -> ();
} from Foreign C;

export {
	quotient:	(Int, Int) -> (Int, Int);
	print:		Int -> ();
} to Foreign C;

quotient (x: Int, y: Int) : (Int, Int) == divide(x, y);
print (x: Int) : () == print << x << newline;

bar()
