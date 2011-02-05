-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "ax0"
import from AxiomLib;

#library IntLib "intcat0.ao"
import from IntLib;

INS : Category == with {
	UFD;
	CoercibleTo T;
}

export Int : INS;

RomanNumber : INS == Int add {
	Rep ==> Int;
	coerce (n: %) : T == {
		import from Int;
		rep(n)::T;
	}
}
