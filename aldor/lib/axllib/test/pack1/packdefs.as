-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------
----
---- packdefs.as:  Packed Types
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

+++ Raw(T) is the type-specific format for values from T.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1995
+++ Keywords:  packed type

--Raw (T: Type) : Type == add;
--!! Raw T ==> T -> Type;

Raw (T: Type) : Type == add {
  Rep == BDFlo;
}

+++ `Packable' provides operations `raw' and `box' for converting
+++ values from a type to its Raw type.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1995
+++ Keywords:  packed type

Packable : Category == BasicType with {
	raw:		% -> Raw %;
	box:		Raw % -> %;
}

+++ `GenericType' indicates that the operations for converting
+++ values from a type to its Raw type are trivial.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1995
+++ Keywords:  packed type

GenericType : Category == Packable with {
	RawType:	% -> %;
	default {
		RawType (x: %) : % == raw x;
		raw (x: %) : Raw % == x;
		box (x: Raw %) : % == x;
	}
}
