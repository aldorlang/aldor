-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--
-- Designed after bug 1087.
--
-- The compiler doesn't re-order definitions anymore because that
-- causes too many problems with non-lazy constants whose value
-- depends on a variable. This test ought to generate errors.

--> testerrs
--> testrun -laxllib -Q3
--> testrun -laxllib -Q0

#include "axllib"

+++ ExplicitDependencies(n) tests the dependency analysis code for
+++ non-lazy constants. The exports A, B, C and D must be defined
+++ (at runtime) in the order D, C, B and then A. This ordering can
+++ be computed by only following explicit dependencies.
ExplicitDependencies(n:SingleInteger):with
{
   A:SingleInteger;
   B:SingleInteger;
   C:SingleInteger;
   D:SingleInteger;
}
== add
{
   import from SingleInteger;

   local foo():SingleInteger == n;


   -- A explicitly depends on B, C and D, and implicitly on C and D.
   A:SingleInteger ==
   {
      local atmp:SingleInteger := C;

      for i in D..(C - 5) repeat
         atmp := atmp + B;

      atmp;
   }


   -- B explicitly depends on C and D, and implicitly on D.
   B:SingleInteger == C - D;


   -- C explicitly depends only on D and has no implicit dependencies.
   C:SingleInteger ==
   {
      local ctmp:SingleInteger;

      ctmp := 1 + (D + D);
      ctmp;
   }


   -- D has no dependencies.
   D:SingleInteger == foo() + foo();
}


+++ ImplicitDependencies(n) tests the dependency analysis code for
+++ non-lazy constants. The exports A, B, C and D must be defined
+++ (at runtime) in the order D, C, B and then A. This ordering can
+++ only be computed by taking implicit dependencies into account.
ImplicitDependencies(n:SingleInteger):with
{
   A:SingleInteger;
   B:SingleInteger;
   C:SingleInteger;
   D:SingleInteger;
}
== add
{
   import from SingleInteger;

   local foo():SingleInteger == n;


   -- A explicitly depends on B, and implicitly on C and D.
   A:SingleInteger ==
   {
      local atmp:SingleInteger := n;

      for i in foo()..(foo() + 5) repeat
         atmp := atmp + (B + 1);

      atmp;
   }


   -- B explicitly depends on C, and implicitly on D.
   B:SingleInteger == C;


   -- C explicitly depends only on D and has no implicit dependencies.
   C:SingleInteger ==
   {
      local ctmp:SingleInteger;

      ctmp := 1 + (D + D);
      ctmp;
   }


   -- D has no dependencies.
   D:SingleInteger == foo() + foo();
}


testExplicit():() ==
{
   import from SingleInteger;
   import from ExplicitDependencies(5);

   print << "Testing explicit dependency checking:" << newline;
   print << "   A = " << A << newline; -- A = 98
   print << "   B = " << B << newline; -- B = 11
   print << "   C = " << C << newline; -- C = 21
   print << "   D = " << D << newline; -- D = 10
   print << newline << newline;
}


testImplicit():() ==
{
   import from SingleInteger;
   import from ImplicitDependencies(5);

   print << "Testing implicit dependency checking:" << newline;
   print << "   A = " << A << newline; -- A = 137
   print << "   B = " << B << newline; -- B = 21
   print << "   C = " << C << newline; -- C = 21
   print << "   D = " << D << newline; -- D = 10
   print << newline << newline;
}


testExplicit();
testImplicit();

