-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"

--> testcomp
--> testrun -l axllib

r: Record(a: Integer, b: Integer == 2) == [1$Integer];

i: Integer == apply(r, a);
j: Integer == apply(r, b);

print<<i<<j<<newline
