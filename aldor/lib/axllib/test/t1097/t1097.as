--> testerrs
--* Subject: [1] bad inference with conditional exports

--@ Bug Number:  bug1097.as 
--@ Fixed  by:  MND   
--@ Tested by:  none 
--@ Summary:    General improvements to the compiler 

-- Command line: none
-- Version: 1.1.7a
-- Original bug file name: badinfer.as

------------------------------  badinfer.as ---------------------------------
--
-- This file produces 2 separate inference problems with conditions (1.1.7a)
-- The second one is very sensitive to small code changes, for example it
-- does not occur if either baz is not local, or if bar does not return a pair.
--
-- % axiomxl -m2 badinfer.as
-- "badinfer.as", line 33: 
--         if R has FiniteField then foo(a:P):P == a quo a;
-- ..................................................^
-- [L33 C51] #1 (Error) There are no suitable meanings for the operator `quo'.
-- The following could be suitable if imported:
--   quo: (P, P) -> P from P, if R has Field
--  
-- "badinfer.as", line 40:                         inv(down a) * a;
--                         ........................^
-- [L40 C25] #2 (Error) There are no suitable meanings for the operator `inv'.
-- The following could be suitable if imported:
--   inv: R -> R from R, if R has Field
--

#include "axllib"

MyCat(R:Ring):Category == with {
	down: % -> R;
	if R has Field then EuclideanDomain;
}

InferenceBugs(R:Ring, P:MyCat R): with {
		foo:	P -> P;
		bar:    P -> (R, P);
} == add { 
	if R has FiniteField then foo(a:P):P == a quo a;

	bar(a:P):(R, P) == { R has Field => (1, baz a); (1, a); }

	if R has Field then {
		local baz(a:P): P == {
			import from R;
			inv(down a) * a;
		}
	}
}

