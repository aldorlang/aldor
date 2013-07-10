-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

Int ==> SingleInteger

import from Int
import from Segment Int
import from ClosedSegment Int
import from List Int


-- This sequence works just fine.
default i:Int;

a : ClosedSegment Int:= 1..10;
l := [i for i in a];

print <<l<<newline;

-- This sequence generates bad foam.
m : List Int :=
	[i for i in b] where
		b := 1..10;

print <<m<<newline;

print <<[x for x in abc]<<newline where abc := 1..2;

Pack1: with
    a:Int
    goo: () -> Int
 == add
     a:Int==1
     goo():Int == foo() where
             foo: ()->Int == ():Int +-> a+1

import from Pack1
print<<goo()<<newline


