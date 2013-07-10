
--> testrun -Q3 -laxllib

#include "axllib"

-- A function that ought to always return false
foo():Boolean == {
   import from SingleInteger;
   for i in 1..10 repeat {
      if (i = 5) then return false;
   }
   return true;
}

-- A function that ought to always return an empty list
bar():List(SingleInteger) == [i for i in 1..10 | foo()];

import from List(SingleInteger);
print << bar();
