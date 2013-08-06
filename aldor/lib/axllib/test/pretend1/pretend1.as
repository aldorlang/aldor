-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib.as"


-- The compiler ought to be able to handle boolean constants better
-- for has tests. This file attempts to test the basic handling.
Foo(T:BasicType): with
{
   foo : T -> T;
}
== add
{
   foo(t:T):T ==
   {
      import from T;
      local tmp:T;

      hasRing == T has Ring;

      if (hasRing) then
      {
         -- The compiler ought to know about + : (T, T) -> T
         tmp := (0 + 1);
      }

      t
   }
}

