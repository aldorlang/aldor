--> testint

-- From original submitted by Manuel Bronstein
-- Problem was missing break statements in interpreter for Word <-> SInt
-- casts. No idea why it wasn't broken on OSF/Solaris machines!
--
-- DO NOT optimise this test: cfold and cast will wipe away the bug.

-- % axiomxl -ginterp badshift.as
--      On OSF/Solaris this returns the correct answer
--         shift(-10399264, -1) = -5199632
--      On Linux this returns a wrong answer
--         shift(-10399264, -1) = -1005328
--
-- Works ok if -fx is used to generate an executable
--

#include "axllib"

import from SingleInteger;

x := -10399264;		-- binary = -100111101010111000100000

print << "shift(" << x << ", -1) = " << shift(x, -1) << newline;

