-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testerrs

#include "axllib.as"


#if TestErrorsToo
-- Test how the compiler deals with applications of operators
-- which have been conditionally defined.
Foo(A:BasicType) : with
{
   f1 : % -> %;
} == add
{
   Rep ==> Integer;


   -- Conditionally defined local function.
   if A has Ring then
   {
      locfun(a:%):% == a;
   }


   f1(a:%):% ==
   {
      b : % := locfun a; -- BUG: what if not(A has Ring).
      b
   }
}
#endif

