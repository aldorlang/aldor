-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "foamlib"
#include "assertlib"

Int ==> SingleInteger

import from Int
import from Segment Int
import from List Int
import from Assert List Int

-- This sequence works just fine.
default i:Int;

a : ClosedSegment Int:= 1..10;
l := [i for i in a];

print <<l<<newline;

m : List Int :=	[i2 for i2 in b] where b := 1..4
assertEquals([1,2,3,4], m)

#if 0
-------

e where defs ==> (apply (lambda defs, e) {d1, d2})
-->

f(x, y) where x := g(a)

(set E2 (pushenv X (env 0)))
(set (EElt 0 E2) (ccall g a))
(call (lex 1 f) (EElt 0 E2) (Lex 0 y)
#endif
