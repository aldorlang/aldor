--> testint
--> testrun -laxllib
--@ Bug Number:  bug1064.as 
--@ Fixed  by:  PAB   
--@ Tested by:  none 
--@ Summary:    Not a bug. The behaviour of mod_X is now documented. 

-- Command line: none
-- Version: 1.1.6
-- Original bug file name: modneg.as

------------------------ modneg.as ---------------------
--
-- It look likes the various mod_X operations give wrong result when
-- one of the first 2 inputs is negative. Are negative inputs supposed
-- to be supported?
-- % axiomxl -fx modneg.as
-- % modneg
-- 2021724253 + 760744993 (mod 2147481359) = 634987887
-- 2021724253 - (-760744993) (mod 2147481359) = 634983309
--                                              ^^^^^^^^^ WRONG!

#include "axllib"

macro Z == SingleInteger;

import from Z;
a := 2021724253;
b := -760744993;
p := 2147481359;
good  := mod_+(a, -b, p);
bad   := mod_-(a, b, p);
print << a << " + " << -b << " (mod " << p << ") = " << good << newline;
print << a << " - (" << b << ") (mod " << p << ") = " << bad << newline;


