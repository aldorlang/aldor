-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Tests the ordering of statements/definitions in a function.

--> testrun -laxllib -Q3
--> testrun -laxllib -Q0

#include "axllib.as"

main():() ==
{
   import from Float;

   f(a: Float): Float == {a*a};
   pi: Float := 3.14159;
   p : Float == f(pi);

   print << "pi**2 = " << p << newline;
}


main();
