#include "algebra"

LogicAtom: Category == Join(ExpressionType, TotallyOrderedType) with {
   negate: % -> %;
   negated?: % -> Boolean;
   positive: % -> %;
}

BooleanAlgebra: Category == ExpressionType with {
   _and: (%, %) -> %;
   _or: (%, %) -> %;
   _not: % -> %;

   false: %;
   true: %;

   true?: % -> Boolean;
   false?: % -> Boolean;
}

-- Test the basic axioms for Atoms
-- not very interesting, but anyways..
LogicAtomTests(LA: LogicAtom): with {
    testAtom: LA -> Boolean;
}
== add {
    testAtom(a: LA): Boolean == {
        not (a = a) => false;
	a ~= a => false;
	negate a ~= a => false;
	negated? a => negate a = positive a;
	negated? a => not negated? negate a;
	not negated? a => negated? negate a;
	not negated? a => a = positive a;
	negate negate a ~= a => false;
	true;
    }
}
