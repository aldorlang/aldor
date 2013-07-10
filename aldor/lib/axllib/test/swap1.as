-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"
#pile

Ag ==> (S: BasicType) -> LinearAggregate S

Swap(X:Ag, Y:Ag, S:BasicType): with
    swap: (X Y S) -> Y X S
  == add
    swap(x:X Y S):Y X S ==
        [[s for s: S in y]$X(S) for y: Y S in x]$Y(X(S))

-- Now we import those domains we will use.

import from List  Integer
import from Array Integer
import from Integer
import from Segment Integer
import from String

-- Let's try to swap list and array levels.

al: Array List Integer :=
        array(list(i+j-1 for i in 1..3) for j in 1..3)

print<<"This is an array of lists: "<<newline<<al<<newline<<newline

import from Swap(Array, List, Integer)

la: List Array Integer := swap(al)

print<<"This is a list of arrays: "<<newline<<la<<newline

