-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile
#include "axllib.as"

t: HashTable(SingleInteger, String) := table(=, +)

t.101 := "Hello"

for i in 100..103 repeat {
	(b, v) := search(t, i, "-nope-");
	print << "Searched for "<< i
              << " and got (" << b << ", " << v << ")"
              << newline
}
