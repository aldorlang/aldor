
--> testgen f

-- Compile with -Ffm -Q5 and check the FOAM for `Negate'. Note that we
-- don't inline `negate'.

#include "axllib"

BaseDomain:with
{
   negate: % -> %;
}
== add
{
   Rep == Boolean;

   negate(x:%):% == per not rep x;
}


ChildDomain:with
{
   negate: % -> %;
   Negate: % -> %;
}
== BaseDomain add
{
   Negate(x:%):% == negate(x);
}



