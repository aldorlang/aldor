
#include "axllib"

--> testerrs

foo():SingleInteger ==
{
   42;
   assert(true); -- Boom
}

import from SingleInteger;
local bar:SingleInteger := foo();

