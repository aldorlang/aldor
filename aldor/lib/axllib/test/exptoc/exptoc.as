-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib"
#pile

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
}

import {
	bar:		() -> ();
} from Foreign C;

export {
	fact:		Int -> Int;
	print:		Int -> ();
} to Foreign C;

fact (x: Int) : Int == if x = 0 then 1 else x * fact(x-1);
print (x: Int) : () == print<<x<<newline;

bar()
