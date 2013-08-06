-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"
#pile

I  ==> SingleInteger;
Ag ==> (S: BasicType) -> LinearAggregate S;

-- This function takes two type constructors as arguments and
-- produces a new function to swap aggregate data structure layers.

swap(X:Ag,Y:Ag)(S:BasicType)(x:X Y S):Y X S == [[s for s in y] for y in x]

al: Array List I := array(list(i+j-1 for i in 1..3) for j in 1..3)
la: List Array I := swap(Array,List)(I)(al)

print<<"This is an array of lists: "<<newline<<al<<newline<<newline
print<<"This is a list of arrays: "<<newline<<la<<newline
