-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib"

#library DemoLib "axldem"
import from DemoLib;

macro {
	SetCategory		== BasicType;

	I			== Integer;
	NNI			== NonNegativeInteger;
	PI			== PositiveInteger;
	Bit			== Boolean;

	NonNegativeInteger	== XNonNegativeInteger;
	PositiveInteger		== XPositiveInteger;

	SemiGroup		== XSemiGroup;
	Monoid			== XMonoid;
	Group			== XGroup;
	AbelianSemiGroup	== XAbelianSemiGroup;
	AbelianMonoid		== XAbelianMonoid;
	AbelianGroup		== XAbelianGroup;
	Ring			== XRing;
	IntegralDomain		== XIntegralDomain;
}

NonNegativeInteger : with {
	coerce:		% -> I;
}
== Integer add {
	coerce (x: %) : I == x pretend I;
}

PositiveInteger : with {
	coerce:		% -> NNI;
}
== Integer add {
	coerce (x: %) : NNI == x pretend NNI;
}

--% unitsKnown

unitsKnown: Category == with;

--% leftUnitary

leftUnitary: Category == with;

--% rightUnitary

rightUnitary: Category == with;

--% noZeroDivisors

noZeroDivisors: Category == with;

--% StepThrough

StepThrough: Category == SetCategory with {
	init:		() -> %;
	nextItem:	% -> Partial %;
}

--% SemiGroup

SemiGroup: Category == SetCategory with {
	*:		(%, %) -> %;
	^:		(%, PI) -> %;
}

--% Monoid

Monoid: Category == SemiGroup with {
	1:		%;
	^:		(%, NNI) -> %;
	one?:		% -> Bit;
	recip:		% -> Partial %;

	default {
		(x: %) ^ (n: PI) : % == x ^ n::NNI;
		one? (x: %) : Bit == x = 1;
		recip (x: %) : Partial % == { one? x => [x]; failed }
	}
}

--% Group

Group: Category == Monoid with {
	inv:		% -> %;
	/:		(%, %) -> %;
	^:		(%, I) -> %;
	unitsKnown;
	conjugate:	(%, %) -> %;
	commutator:	(%, %) -> %;

	default {
		(x: %) / (y: %) : % == x * inv y;
		(x: %) ^ (n: NNI) : % == x ^ n::I;
		recip (x: %) : Partial % == [inv x];
		conjugate (p: %, q: %) : % == inv q * p * q;
		commutator (p: %, q: %) : % == inv p * inv q * p * q;
	}
}

--% AbelianSemiGroup

AbelianSemiGroup: Category == SetCategory with {
	+:		(%, %) -> %;
	*:		(PI, %) -> %;
}

--% AbelianMonoid

AbelianMonoid: Category == AbelianSemiGroup with {
	0:		%;
	*:		(NNI, %) -> %;
	zero?:		% -> Bit;

	default {
		(n: PI) * (x: %) : % == n::NNI * x;
		zero? (x: %) : Bit == x = 0;
	}
}

--% CancellationAbelianMonoid

CancellationAbelianMonoid: Category == AbelianMonoid with {
	subtractIfCan:	(%, %) -> Partial %;
}

--% AbelianGroup

AbelianGroup: Category == CancellationAbelianMonoid with {
	-:		% -> %;
	-:		(%, %) -> %;
	*:		(I, %) -> %;

	default {
		(x: %) - (y: %) : % == x + (-y);
		(n: NNI) * (x: %) : % == n::I * x;
		subtractIfCan (x: %, y: %) : Partial % == (x-y) :: Partial %;
	}
}

--% Rng

Rng: Category == Join(AbelianGroup, SemiGroup);

--% LeftModule

LeftModule(R: Rng): Category == AbelianGroup with {
	*:		(R, %) -> %;
}

--% RightModule

RightModule(R: Rng): Category == AbelianGroup with {
	*:		(%, R) -> %;
}

--% Ring

Ring: Category == Join(Rng, Monoid, LeftModule %) with {
	characteristic:	() -> NNI;
	coerce:		I -> %;
	unitsKnown;

	default {
		-- Check that we can package call from % in the defaults.
		coerce (n: I) : % == n * 1$%;
	}
}

--% BiModule

BiModule(R: Ring, S: Ring): Category == Join(LeftModule R, RightModule S) with
{
	leftUnitary;
	rightUnitary;
}

--% EntireRing

EntireRing: Category == Join(Ring, BiModule(%, %)) with {
	noZeroDivisors;
}

--% CommutativeRing

CommutativeRing: Category == Join(Ring, BiModule(%, %)) with;

--% Module

Module(R: CommutativeRing) : Category == BiModule(R, R) with;

--% Algebra

Algebra(R: CommutativeRing) : Category == Join(Ring, Module R) with {
	coerce:		R -> %;

	default {
		-- Check that we need not package call from %.
		coerce (x: R) : % == x * 1;
	}
}

--% IntegralDomain

IntegralDomain: Category == Join(CommutativeRing, Algebra %, EntireRing) with {
	exquo:		(%, %) -> Partial %;
	unitNormal:	% -> Record(unit: %, canonical: %, associate: %);
	unitCanonical:	% -> %;
	associates?:	(%, %) -> Bit;
	unit?:		% -> Bit;

	default {
		UCA ==> Record(unit: %, canonical: %, associate: %);
		import from UCA, Partial %;
		unitCanonical (x: %) : % == unitNormal(x).canonical;
		recip (x: %) : Partial % ==
			if zero? x then failed else ((exquo)(1, x));
		unit? (x: %) : Bit == not (recip x case "failed");
	}
}
