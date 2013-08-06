-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

-- Tests of various sections of the optimiser.

--> testcomp -OQinline-all
--> testrun -OQinline-all -l axllib

-- This gives the env. merger a workout
LoopList(T: BasicType): with
     breadthfirst: (T -> List T, T -> Boolean) -> T -> (List T)
  == add
      import from List T
      breadthfirst(nex: T->List T, pred: T->Boolean)(x:T):List T ==
          bfs(ll:List T):List T ==
            empty? ll => nil$(List T)
            pred (first ll) => cons(first ll, bfs concat(rest ll, nex first ll))
            bfs concat(rest ll, nex first ll)
          bfs [x]

-- Silly Driver
import from SingleInteger
gen(t: SingleInteger): List SingleInteger == 
	if t < 20 then [ (i*t+7) for i in 1..5] else [];
test(t: SingleInteger): Boolean == (t mod 5) = 0;

foo():() ==
	import from LoopList SingleInteger;
	import from List SingleInteger;
	print << breadthfirst(gen,test)(0);
	print << newline

foo()
