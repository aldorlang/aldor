
#include "foamlib"
macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil() @ Ptr pretend S;
	Nil?(S)(x)	== x @ S pretend Ptr = nil();
	Reserved	== Pointer;
	DomainRep       == Pointer;
}

macro {
	ObjectTyper	== () -> Int;
	ObjDomain	== 0;
	ObjCategory	== 10;
}

import from Pointer;

-- Value is used for types stored in a single word.  All domain exports
-- must be storable in this type.
Value: BasicType == Ptr add {};

Box: Conditional with {
	new:		Value -> %;
		++ new(val) creates an initialized box.

	value:		% -> Value;
		++ value(box) returns the value from the box.

	setVal!:	(%, Value) -> ();
		++ setVal!(box, val) sets the value in a box.

	nullBox:	() -> %;
		++ nullBox() returns the distinguished empty box,
		++ which cannot hold a value.
}
== add {
	Rep ==> Record(value: Value);

	import from Rep;

	new	(v: Value): %		== per [v];
	value	(b: %): Value		== rep(b).value;
	setVal!	(b: %, v: Value): ()	== rep(b).value := v;
	nullBox	(): %			== Nil %;

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}
