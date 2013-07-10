--> testerrs
--> testcomp -Dfix
--@ Bug Number:  bug1094.as 
--@ Fixed  by:  PAB   
--@ Tested by:  none.as 
--@ Summary:    Compiler now catches cases where dependent types contain variables 

-- Command line: none
-- Version: 1.1.7 (Solaris)
-- Original bug file name: segfault.as

----------------------------- segfault.as ---------------------------
--
-- This is a result of fix1085: this now seg faults during the .as to .ao phase
-- (1.1.7 for Solaris)
--
-- % axiomxl segfault.as
-- Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).

#include "axllib"

macro REC == Record(S:BasicType, L:List S);

explode1(r:REC):(T:BasicType, L:List T) == {
#if fix
	(x, y) == explode r;
#else
	(x, y) := explode r;
#endif
	(x, y);
}

foo(r:REC):() == foo0(explode1 r);

foo0(S:BasicType, l:List S):() == print << l;


