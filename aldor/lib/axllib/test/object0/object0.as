-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testrun -O -l axllib

#include "axllib.as"

check (T: Order, t: T) : () == {
	if T has OrderedAbelianMonoid then {
		if t > 0 then print << "[" << t << "]";
	}
}

f(): () == {
	import from SingleInteger, SingleFloat, DoubleFloat, Ratio Integer;
	import from Character;

	x: List Object Order := [
		object(Character, char "g"),
		object(Integer, -13),
		object(SingleInteger, 2),
		object(SingleFloat, 1.25),
		object(DoubleFloat, -0.4),
		object(Ratio Integer, 2/3)
	];

	for ob in x repeat
		check avail ob;
	print << newline;
}

f();
