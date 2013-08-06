--@ Bug Number:  bug1075.as 
--@ Fixed  by:  MND   
--@ Tested by:  none 
--@ Summary:    General improvements to the compiler 

-- Command line: axiomxl -q1 mytype.as mycat.as buggy.as
-- Version: 1.1.6
-- Original bug file name: badbug.as

------------------------------   badbug.as   ------------------------------
--
-- To reproduce this bug, you must extract the 3 files:
--         mytype.as     mycat.as   and  buggy.as
--
-- Compiling them on the same command line produces a compile-time error:
-- % axiomxl -q1 mytype.as mycat.as buggy.as
-- 
-- mytype.as:
-- 
-- mycat.as:
-- 
-- buggy.as:
-- "buggy.as", line 15: 
--                 for i in 1..m repeat basis := cons(g m, basis);
-- ..........................^
-- [L15 C27] #1 (Error) There are no suitable meanings for the operator `..'.
-- 
--
-- Compiling the 3 files separately does seem to work however,
-- but someone is producing bad code somewhere, because in the
-- instance when we ran into this, the resulting code seg faults in
-- some domain initialization function.
-- This explains the high priority -- on this bug.
--
------------------------   buggy.as   -----------------------
#include "axllib"

#library lib1 "t1075a.ao"
#library lib2 "t1075b.ao"
import from lib1, lib2;

macro SI  == SingleInteger;

Foo(P:MyCat): with { foo: P -> List MyType } == add {
	foo(a:P):List MyType == {
		import from SI, MyType;
		basis:List MyType := empty();
		m := #basis;
		for i in 1..m repeat basis := cons(g m, basis);
		basis;
	}
}

