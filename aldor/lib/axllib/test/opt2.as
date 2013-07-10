-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O
--> testrun -O -l axllib

------------------------------- int.as ----------------------------
--
-- Optimizing changes the value of 6/3:
--
-- % axiomxl -Q1 -Fx int.as
-- % int
-- 6 / 3 = 2
--
-- % axiomxl -Q2 -Fx int.as
-- % int
-- 6 / 3 = 6
--

#include "axllib.as"

IntegerCategory: Category == IntegerNumberSystem with {
	exactQuotient: (%, %) -> Partial %;
	default {
		exactQuotient(x:%, y:%):Partial(%) == {
			(q, r) := divide(x, y);
			zero? r => [q];
			failed
		}
	}
};

extend Integer: IntegerCategory == add {};

f() : () == {
	import from Integer, Partial Integer;

	print << "6 / 3 = " << retract exactQuotient(6, 3) << newline;
}

f();

