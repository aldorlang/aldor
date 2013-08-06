--> testcomp 
--@ Bug Number:  bug1093.as 
--@ Fixed  by:  PAB   
--@ Tested by:  if5.as 
--@ Summary:    Beefed-up conditional handling 

-- Command line: none
-- Version: 1.1.7
-- Original bug file name: badinfer.as

------------------------------- badinfer.as ----------------------------------
--
-- This is a result of fix966: the following does not compile any more.
-- This is a crucial bug, which kills lots of code using conditional exports,
-- and I don't have a workaround in sight.
--
-- % axiomxl -m2 badinfer.as
-- "badinfer.as", line 20:         ident(p:P):P == gcd(p, p);
--                         ........................^
-- [L26 C25] #1 (Error) There are no suitable meanings for the operator `gcd'.
-- The following could be suitable if imported:
--   gcd: (P, P) -> P from P, if R has GcdDomain
--

#include "axllib"

GcdDomain: Category == Ring with { gcd: (%, %) -> % };

Foo(R:Ring):Category == with {
	foo: % -> %;
	if R has GcdDomain then GcdDomain;
	if R has Field then GcdDomain;
}

Bar(R:Field, P:Foo R): with { ident: P -> P } == add {
	ident(p:P):P == gcd(p, p);
}


