------------------------------------
--
-- description:
--   CheckedList performs no bound checking
--   with apply
--
--
-- executed via:
--   aldor -ginterp bugCklst.as
--
------------------------------------

#include "aldor"

import from MachineInteger;
import from CheckingList MachineInteger;
import from Character;
import from TextWriter;

-- [5,6,7].2 gives 6, which is correct
stdout << ([5,6,7].2) << newline;


-- [5,6,7].4 gives a segmentation fault, but should probably
-- throw a ListException, since is a _CheckedList_

try {

	stdout << ([5,6,7].4) << newline;

} catch E in {
	E has ListExceptionType => 
              {
                import from String; 
                stdout << "List Exception" << newline;
              }
	never;
}

