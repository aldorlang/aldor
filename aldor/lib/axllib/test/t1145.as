--> testint
--> testint -q3
--> testrun -l axllib
--> testrun -q3 -l axllib
#include "axllib"

import from SingleInteger;

-- this constant is 2^40, no problem on a 64-bit machine
big():Boolean == {
   i:Integer:=1099511627776;
   max@SingleInteger ::Integer > i
}
{

not big()=> {
   print << "q = " << "1099511627776" << newline;
}
   print << "q = " << 1099511627776 << newline;
}
-- On a DEC-alpha I get the following:
-- % axiomxl -q1 -fx optbug.as
-- % optbug
-- q = 1099511627776
-- 
-- % axiomxl -q2 -fx optbug.as
-- % optbug
-- q = 0
-- 
-- Looking at the generated C-code, I see for -q1:
--   T5 = fiCCall1(FiWord, l0->X10_integer, (FiWord) "1099511627776");
-- and for -q2, the print statement got inlined into:
--      fiCCall3(FiWord, T5, (FiWord) "q = ", (FiWord) 1, (FiWord) 0);
--      T11 = formatSInt(0);
-- SO THE OPTIMIZER "ATE" THE HIGH 32-BIT PART OF MY CONSTANT!



