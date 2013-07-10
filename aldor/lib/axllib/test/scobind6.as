-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib -Q0
--> testrun -l axllib -Q3

#include "axllib"

main():() ==
{
   import from SingleInteger;

   -- Test scobind/tinfer of for loops
   for 1 in 1..10 repeat
   {
      print << 1$SingleInteger; --------- An import
      print << " <> ";
      print << 1; ----------------------- An implicit local
      print << newline;
   }
}

main();

