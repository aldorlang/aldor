--> testcomp
--> testgenc -q1 
--@ Bug Number:  bug1085.as 
--@ Fixed  by:  PAB   
--@ Tested by:  t1024.as 
--@ Summary:    Check for mutiple values within MFmts (Same problem as bug1024) 

-- Command line: axiomxl -fc -q1 case72.as
-- Version: 1.1.6
-- Original bug file name: case72.as

----------------------------- case72.as ---------------------------
--
-- The explode1/bang0 workaround to bug1082 allows me to produce
-- foam code, but not C code...
--
-- % axiomxl -fc -q1 base72.as
-- Compiler bug...Bug: Bad case 72 (line 2074 in file ../src/genc.c).
--

#include "axllib"

macro REC == Record(S:BasicType, L:List S);

explode1(r:REC):(T:BasicType, L:List T) == {
	(x, y) == explode r;
	(x, y);
}

foo(r:REC):() == foo0(explode1 r);

foo0(S:BasicType, l:List S):() == print << l;

