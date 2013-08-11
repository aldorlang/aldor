-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

SI ==> SingleInteger;

D : with {
	f: SI -> SI;
}
== add {
	f(n: SI): SI == {
		sum: SI := 0;
		n ~= 0 => 0;
		for i in 1..n repeat sum := sum + i;
		sum;
	}
}
