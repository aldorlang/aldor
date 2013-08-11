-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "ax0"
import from AxiomLib;

T : with == add;

export Fact: (D: Type) -> with {
	*:	(D, %) -> %;
}

UFD : Category == with {
	CoercibleTo OutputForm;
	sqFr:	% -> Fact %;
}

