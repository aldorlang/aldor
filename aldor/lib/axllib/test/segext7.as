-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"

NNI ==> NonNegativeInteger;

D : with {
	f: NNI -> NNI;
}
== add {
	f(n: NNI): NNI == {
		sum: NNI := 0;
		n ~= 0 => 0;
		for i in 1..n repeat sum := sum + i;
		sum;
	}
}
