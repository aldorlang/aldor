--> testint
--> testrun -laxllib

-- From the original bug report submitted by Manuel Bronstein

-- XByte seems to behave like signed bytes rather than unsigned
-- This cause the conversion XByte <--> SInt not to be inverses of each other
-- In the following, one would expect x and y to be the same
-- (and byte's to be unsigned)
--
-- % axiomxl -ginterp xbyte.as 
-- x = 250, y = -6

#include "axllib"

import from Machine;

x:SingleInteger := 250;
b := convert(x::SInt)@XByte;		-- convert x to XByte
y := (convert(b)@SInt)::SingleInteger;	-- convert back to SingleInteger

print << "x = " << x << ", y = " << y << newline;

