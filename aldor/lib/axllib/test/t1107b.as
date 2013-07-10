--* Subject: [1] improper "missing export" complaint

--@ Bug Number:  bug1107.as 
--@ Fixed  by:  PAB   
--@ Tested by:  exp.as 
--@ Summary:    Corrected handling of conditional exports (I hope) 

-- Command line: none
-- Version: 1.1.8
-- Original bug file name: missing.as

------------------------------ missing.as ---------------------------
--
-- TO REPRODUCE THIS BUG, YOU MUST BREAK THIS FILE INTO 2 FILES foo.as
-- AND bar.as, OTHERWISE THE BUG DOES NOT APPEAR
--
-- % axiomxl -m2 foo.as
-- % axiomxl -m2 bar.as
-- tfIf:<* If <* General (* (* (* R *) (* Field *) *) *) *> <* General (* (* (* (* bar *) (* (* -> *) (* (* a *) (* % *) *) (* SingleInteger *) *) *) (* (* (* (* a *) (* % *) *) *) (* SingleInteger *) (* *) *) *) *) *> <* Multiple *> *>
-- "bar.as", line 9: Baz(R:Ring):Bar R == add {
--                   .....................^
-- [L9 C22] #1 (Error) The domain is missing some exports.
--         if R has Field then
--                 The domain is missing some exports.
--
------------------------   bar.as   -----------------------
#include "axllib"

#library lib "t1107a.ao"
import from lib;

macro SI  == SingleInteger;

Baz(R:Ring):Bar R == add {
	macro Rep == SI;

	import from Rep;

	(a:%) = (b:%):Boolean == false;
	(p:TextWriter) << (a:%):TextWriter == p;
	foo(n:SI):% == per n;
	sample:% == foo 1;

-- THISE IS CLAIMED TO BE "missing" BY 1.1.8,
-- ALTHOUGH IT IS PROVIDED AS DEFAULT BY Bar(R)
-- COMPILES OK IF THIS LINE IS UNCOMMENTED, GIVES "missing export" OTHERWISE
--	if R has Field then bar(a:%):SI == 0;
}



