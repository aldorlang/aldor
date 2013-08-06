-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#include "axllib.as"
#pile

R ==>  Record(i: Integer, b: Boolean)
import from R
import from Integer
import from Boolean

l:  List R := [[1, true], [2, false], [3, true]]

for x in l repeat {
	print<<"i = "<<x.i<<", b = "<<x.b<<newline
}
