-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

I ==> Integer;

f(n: I): I == {
	sum: I := 0;
	n ~= 0 => 0;
	for i in 1..n repeat sum := sum + i;
	sum;
}
