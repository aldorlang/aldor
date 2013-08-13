------------------------------------
--
-- description:
--   the name of the return value
--   is in scope of the function body
--
--
-- executed via:
--   aldor -ginterp bugReturn.as
--
------------------------------------

#include "aldor.as"

import from Integer;

foo(i:Integer):(r:Integer) == {
	r;
}

foo(3);
