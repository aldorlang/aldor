-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib.as";

AA ==> BasicType -> BasicType;
Int ==> SingleInteger;
import from List Int;

foo() : with {
	comp : (AA, AA) -> AA;
}
== add {
	comp (r: AA, s: AA) : AA == {
		loco (bt: BasicType) : BasicType == r s bt;
		loco;
	}
}

G == comp(List, List)$foo();
HH == G(Int);

if HH has with { list: Tuple List Int -> % } then {
	import from HH;
	lll := list(list(1,2,3,4),list(8,9));
	print << lll << newline;
}
else
	print << "too bad" << newline;
