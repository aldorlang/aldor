
-- Test exception handling with cartesian product functions whose result
-- is embedded in a multi.

--> testcomp
--> testrun -l axllib
--> testrun -Q3 -l axllib
--> testint


#include "axllib"


-- Abbreviations.
Z ==> Integer;


-- Define the exception used in the examples.
define ZeroDivideException(R:Ring):Category == with;
ZeroDivide(R:Ring):ZeroDivideException(R) == add;


-- Simple display function.
show(fn:String, numer:Z, denom:Z, divisor:Z, remainder:Z):() ==
{
   print << fn << ": ";
   print << numer << "/" << denom << " = ";
   print << divisor << " r " << remainder << newline;
}


-- Multi-valued division with result as a multi.
div(a:Z, b:Z):Cross(Z, Z) ==
{
   zero? b => throw ZeroDivide(Z);
   divide(a, b);
}


-- Try block whose value is ignored and whose try-expression is
-- multi-valued in a sequence.
try0(numer:Z, denom:Z):() ==
{
   local divisor, remainder:Z;

   try { (divisor, remainder) := div(numer, denom); }
   catch E in
   {
      E has ZeroDivideException(Integer) =>
      {
         print << "*** Caught exception: division by zero" << newline;
         divisor   := numer;
         remainder := 0;
      }
      never;
   }


   -- Show the result.
   show("try0", numer, denom, divisor, remainder);
}


-- Try block whose value is ignored and whose try-expression is
-- multi-valued and not in a sequence.
try1(numer:Z, denom:Z):() ==
{
   local divisor, remainder:Z;

   try (divisor, remainder) := div(numer, denom)
   catch E in
   {
      E has ZeroDivideException(Integer) =>
      {
         print << "*** Caught exception: division by zero" << newline;
         divisor   := numer;
         remainder := 0;
      }
      never;
   }


   -- Show the result.
   show("try1", numer, denom, divisor, remainder);
}


-- Try block which is multi-valused and whose try expression is
-- multi-valued in a sequence.
try2(numer:Z, denom:Z):() ==
{
   local divisor, remainder:Z;

   (divisor, remainder) := try { div(numer, denom); }
   catch E in
   {
      E has ZeroDivideException(Integer) =>
      {
         print << "*** Caught exception: division by zero" << newline;
         (numer, 0);
      }
      never;
   }


   -- Show the result.
   show("try2", numer, denom, divisor, remainder);
}


-- Try block which is multi-valused and whose try expression is
-- multi-valued and not in a sequence.
try3(numer:Z, denom:Z):() ==
{
   local divisor, remainder:Z;

   (divisor, remainder) := try div(numer, denom)
   catch E in
   {
      E has ZeroDivideException(Integer) =>
      {
         print << "*** Caught exception: division by zero" << newline;
         (numer, 0);
      }
      never;
   }


   -- Show the result.
   show("try3", numer, denom, divisor, remainder);
}


tryit(fn: (Z, Z) -> ()):() ==
{
   import from Z;

   fn(23, 7);
   fn(23, 0);
   print << newline << newline;
}


main():() ==
{
   tryit(try0);
   tryit(try1);
   tryit(try2);
   tryit(try3);
}


main();

