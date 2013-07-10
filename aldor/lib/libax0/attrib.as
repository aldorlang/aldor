-----------------------------------------------------------------------------
----
---- attrib.as:  Attribute definitions used in the Axiom library.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

import from AxiomLib;
inline from AxiomLib;

macro {
	Attribute X == X: Category == with;
}

Attribute nil;
Attribute infinite;
Attribute arbitraryExponent;
Attribute approximate;
Attribute complex;
Attribute shallowMutable;
Attribute canonical;
Attribute noetherian;
Attribute central;
Attribute partiallyOrderedSet;
Attribute arbitraryPrecision;
Attribute canonicalsClosed;
Attribute noZeroDivisors;
Attribute rightUnitary;
Attribute leftUnitary;
Attribute additiveValuation;
Attribute unitsKnown;
Attribute canonicalUnitNormal;
Attribute multiplicativeValuation;
Attribute finiteAggregate;
Attribute shallowlyMutable;

Attribute commutative(T: Type);
