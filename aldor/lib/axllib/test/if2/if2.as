-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen y

#include "axllib.as"
#pile

PType ==> with {
	Ring;
	if R has Field then
		Field;
	x: %;
};

P(R: Ring) : PType == add pretend PType;
