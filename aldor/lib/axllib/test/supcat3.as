-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

macro {
	SI	== SingleInteger;
	NNI	== NonNegativeInteger;
	I	== Integer;
}

var(n: I) : SparseUnivariatePolynomial I == {
	import from NNI;
	c: I := 1;
	monomial(c, n::NNI);
}
