-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

RomanNumber : IntegerNumberSystem == Integer add {
	Rep ==> Integer;
	convert(n: %): String ==
		convert(rep(n + 1))$Integer;
}
