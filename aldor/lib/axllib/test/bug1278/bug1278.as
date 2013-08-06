--> testerrs -q2

------------------------------ bug1278.as ---------------------------------

#include "axllib"

-- This application of search() generates the following compile-time error:
-- (Fatal Error) Looking for `apply' with code `569733829'.  Export not found.
asserted?(props:HashTable(String, Boolean), prop:String):Boolean ==
{
   (got?, result) := search(props, prop, false);
   result;
}


main():() ==
{
   local tab:HashTable(String, Boolean) := table();

   print << "asserted?(fred) = " << asserted?(tab, "fred") << newline;
}


main();

