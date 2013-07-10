-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

macro {
	MyCat	== SetCategory;
	MyRep	== Integer;
}

MyDom : MyCat == MyRep add {
	Rep ==> MyRep;
	import MyCat from MyRep;
	coerce(n: %): OutputForm ==
		rep(n)::OutputForm;
}
