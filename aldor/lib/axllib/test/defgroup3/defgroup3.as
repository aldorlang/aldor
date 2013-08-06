-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Designed to test the collection of definitions for def-group analysis

--> testrun -laxllib -Q3
--> testrun -laxllib -Q0

#include "axllib"


Foo(n:SingleInteger):with
{
   foo: () -> SingleInteger;
}
== add
{
   print << "*** Initialising Foo ..." << newline;

   foo():SingleInteger == n;

   -- We used to reverse the order of these statements.
   {
      print << "   Step 1 ..." << newline;
      print << "   Step 2 ..." << newline;
      print << "   Step 3 ..." << newline;

      -- These were okay because two reverses undid the damage.
      {
         print << "      Step 3a ..." << newline;
         print << "      Step 3b ..." << newline;
         print << "      Step 3c ..." << newline;
      }

      print << "   Step 4 ..." << newline;
      print << "   Step 5 ..." << newline;
      print << "   Step 6 ..." << newline;
   }

   print << "*** Foo is now initialised." << newline;
}


main():() ==
{
   import from SingleInteger;
   import from Foo(42);

   local result:SingleInteger := foo();

   print << "The answer is " << result << newline;
}


main();
