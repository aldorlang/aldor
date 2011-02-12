-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

macro {
	SI	== SingleInteger;
	NNI	== NonNegativeInteger;
	I	== Integer;
}

var(n: SI) : SparseUnivariatePolynomial I == {
	import from I, NNI;
	monomial(1$I, n::I::NNI);
}
