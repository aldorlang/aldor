-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -Q2 -l axllib
--> testrun -Q2 -Qno-inline -l axllib
----------------------------------------------------------------------

#include "axllib"

SI ==> SingleInteger;

----------------------------------------------------------------------

define Cat:Category == with
{
   foo : () -> SI;

   default { foo():SI == 42; }
}

----------------------------------------------------------------------

A:Cat == add;


B:Cat == add
{
   import from SI;
   foo():SI == 84;
}

----------------------------------------------------------------------

X:Cat with == add;

Y:Cat with == add
{
   import from SI;
   foo():SI == 84;
}

----------------------------------------------------------------------

main():() ==
{
   import from SI;

   print << "foo()$A = " << foo()$A;
   print << " (ought to be " << foo()$X << ")" << newline;

   print << "foo()$B = " << foo()$B;
   print << " (ought to be " << foo()$Y << ")" << newline;
}

main();

----------------------------------------------------------------------

