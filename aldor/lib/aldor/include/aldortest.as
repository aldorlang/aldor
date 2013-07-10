---------------------------- aldortest.as ---------------------------
--
-- Header file for libaldor test files
--
-- Copyright (c) Manuel Bronstein 1998-2001
-- Copyright (c) INRIA 2000-2001, Version 0.1.13
-- Logiciel LibAldor (c) INRIA 2000-2001, dans sa version 0.1.13
---------------------------------------------------------------------

#include "aldorio"

-- func returns true if test is successful, false otherwise
local aldorTest(name:String, func:() -> Boolean):Boolean == {
	stderr << "    testing " << name << "...                ";
	ok := func();
	if ok then stderr << "OK"; else stderr << "ERROR";
	stderr << newline;
	ok;
}

