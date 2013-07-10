------------------------------ session2.as --------------------------------
--
-- Second interactive session of "First Course on Aldor and Aldorlib"
--
-- You must first run
--   % aldor sort0.as
-- before running this session
--

#include "aldor"
#include "aldorinterp"

-- This tells the compiler that the file sort0.ao is interesting
#library Sort0 "sort0.ao"

-- Libraries are imported in the same way as domains:
import from Sort0

-- bubbleSort! should now be in scope.
bubbleSort!

-- Now we try something:
bubbleSort! [4,1]

-- whoops, we haven't imported from MachineInteger yet.
import from MachineInteger
bubbleSort!([4,3,1,5,7])

-- Or arrays...
import from Array MachineInteger

-- This ought to work...
bubbleSort!([4,3,1,5,7])

-- How about a big array:
bubbleSort!([x for x in 100..1 by -1])

-- The built-in sort! from Array MachineInteger should be somewhat faster:
sort!([x for x in 100..1 by -1])

