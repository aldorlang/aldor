--> testrun -laxllib
--> testint
--@ Bug Number:  bug1029.as 
--@ Fixed  by:  PAB   
--@ Tested by:  limits.as 
--@ Summary:    Handled case where int not immediate but <less than SIntMax 

-- Command line: axiomxl -Fx badint.as
-- Version: 1.1.3
-- Original bug file name: badint.as

-----------------------------  badint.as ----------------------------------
--
-- Coercing a SingleInteger to Integer and back produces a different number:
--
-- axiomxl -Fx badint.as
-- badint
-- a,b,c = 1551877902,1551877902,228600
--

#include "axllib.as"

import from SingleInteger, Integer;

a:SingleInteger := 1551877902;    -- smaller than max()@SingleInteger
b:Integer := a::Integer;
c:SingleInteger := b::SingleInteger;

print << "a,b,c = " << a << "," << b << "," << c << newline;


