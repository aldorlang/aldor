-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib"

macro {
	SetCategory		== BasicType;

	I			== Integer;
	NNI			== NonNegativeInteger;
	PI			== PositiveInteger;

	NonNegativeInteger	== XNonNegativeInteger;
	PositiveInteger		== XPositiveInteger;

	SemiGroup		== XSemiGroup;
	Monoid			== XMonoid;
	Group			== XGroup;
	AbelianSemiGroup	== XAbelianSemiGroup;
	AbelianMonoid		== XAbelianMonoid;
}


NonNegativeInteger : Join(Monoid, AbelianMonoid) with {
	coerce:		% -> I;
}
== Integer add {
	(n: NNI) * (x: %) : % == error "n * x";
	(x: %) ^ (n: NNI) : % == error "x ^ n";
	coerce (x: %) : I == x pretend I;
}

PositiveInteger : Join(SemiGroup, AbelianSemiGroup) with {
	coerce:		% -> NNI;
}
== Integer add {
	(n: PI) * (x: %) : % == error "n * x";
	(x: %) ^ (n: PI) : % == error "x ^ n";
	coerce (x: %) : NNI == x pretend NNI;
}




SemiGroup: Category == SetCategory with {
	*:		(%, %) -> %;
	^:		(%, PI) -> %;
}

Monoid: Category == SemiGroup with {
	1:		%;
	^:		(%, NNI) -> %;

	default {
		(x: %) ^ (n: PI) : % == x ^ n::NNI;
	}
}

Group: Category == Monoid with {
	inv:		% -> %;
	/:		(%, %) -> %;
	^:		(%, I) -> %;

	default {
		(x: %) / (y: %) : % == x * inv y;
		(x: %) ^ (n: NNI) : % == x ^ n::I;
	}
}

AbelianSemiGroup: Category == SetCategory with {
	+:		(%, %) -> %;
	*:		(PI, %) -> %;
}

AbelianMonoid: Category == AbelianSemiGroup with {
	0:		%;
	*:		(NNI, %) -> %;

	default {
		(n: PI) * (x: %) : % == n::NNI * x;
	}
}
