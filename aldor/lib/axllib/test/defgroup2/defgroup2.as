-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Designed to test the detection of cycles in the dependency graph.

--> testerrs

#include "axllib"

+++ CyclicDependencies(n) tests the dependency analysis code for
+++ non-lazy constants. The exports B, C and D form a cycle in the
+++ dependency graph and so it is not possible to compute an order
+++ for their definition at runtime.
CyclicDependencies(n:SingleInteger):with
{
   A:SingleInteger;
   B:SingleInteger;
   C:SingleInteger;
   D:SingleInteger;
   E:SingleInteger;
}
== add
{
   import from SingleInteger;

   local foo():SingleInteger == n;


   -- A explicitly depends only on B, and implicitly on C, D and E.
   A:SingleInteger ==
   {
      local atmp:SingleInteger := n;

      for i in 2..5 repeat
         atmp := atmp + B;

      atmp;
   }


   -- B explicitly depends only on C, and implicitly on D, E and itself.
   B:SingleInteger == C + 3;


   -- C explicitly depends only on D and implicitly on E and B.
   C:SingleInteger ==
   {
      local ctmp:SingleInteger;

      ctmp := 1 + (D + D);
      ctmp;
   }


   -- D explicitly depends on E and B (forming a cycle).
   D:SingleInteger ==
   {
      local dtmp:SingleInteger := 1 + E + B;
      dtmp;
   }


   -- E has no dependencies.
   E:SingleInteger == foo() + foo();
}


testCyclic():() ==
{
   import from SingleInteger;
   import from CyclicDependencies(5);

   print << "Testing cyclic dependency checking:" << newline;
   print << "   A = " << A << newline; -- 109
   print << "   B = " << B << newline; -- 26
   print << "   C = " << C << newline; -- 13
   print << "   D = " << D << newline; -- 11 (B = 0)
   print << "   E = " << E << newline; -- 10
   print << newline << newline;
}


testCyclic();

