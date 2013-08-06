-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c
--> testcomp

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fo -Q1 nilrec.as
-- Version: 1.1.3
-- Original bug file name: nilrec.as

------------------------------- nilrec.as ----------------------------------
--
-- This compiles "ok" at -Q2 (although this causes runtime problems),
-- but causes an internal compiler error at -Q1:
--
-- % axiomxl -Fo -Q2 nilrec.as
--
-- % axiomxl -Fo -Q1 nilrec.as
-- Compiler bug...Bug: Bad case 72 (line 2066 in file ../src/genc.c).
--

#include "axllib.as"

NilRec(T:Tuple Type): with { rec: % -> Record T } == add {
	macro Rep == Pointer;
	rec(r:%):Record T == rep(r) pretend Record T;
}


Foo(T:Type): with { foo: % -> T } == add {
	macro Rep == NilRec T;
	foo(p:%):T == {
		import from Rep, Record T;
		explode rec rep p;
	}
}
