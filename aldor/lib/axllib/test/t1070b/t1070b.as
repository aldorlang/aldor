--@ Bug Number:  bug1070.as 
--@ Fixed  by:  MND   
--@ Tested by:  none 
--@ Summary:    General improvements to the compiler 

-- Command line: none
-- Version: 1.1.6
-- Original bug file name: baddef.as

---------------------- baddef.as ---------------
--
-- This compiles fine if both categories are in the same file.
-- If however we break this into baddef.as and baddef2.as, then
-- we get "Default arguments strike again!"
-- Note that the bug disappears if the line
--	default foo(a:Z, n:Z):Z == a + n;
-- is replaced by
--	default foo(a:Z, n:Z == 0):Z == a + n;
-- but it would be very inconvenient if the default parameter values have
-- to be repeated for each implementation of an exported function. If that
-- is required, why does it work ok in a single file?
--
-- % axiomxl baddef.as
-- % axiomxl -m2 baddef2.as
-- "baddef2.as", line 11:         default bar(m:Z):Z == foo m;
--                        ..................................^
-- [L11 C35] #1 (Error) Argument 1 of `foo' did not match any possible parameter type.
--     The rejected type is SingleInteger.
--     Expected type a: SingleInteger, n: SingleInteger.
--

#include "axllib"

#library bad "t1070a.ao"
import from bad;

macro Z == SingleInteger;

Bar:Category == Foo with {
	bar: Z -> Z;
	default bar(m:Z):Z == foo m;
}


