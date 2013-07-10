-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp 

#include "ax0"
import from AxiomLib;

f(t: Type, g: (t,t)->Boolean, l: List(t)) : List(t) == {
	n := #l;
	l := reverse l;
	l := reverse! l;
	l := setrest!(l,l);
	sort(g,l);
}
