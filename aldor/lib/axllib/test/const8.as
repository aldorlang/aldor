-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Tests the ordering of statements/definitions in a domain export.

--> testerrs

#include "axllib.as"

Foo:with
{
   foo: () -> Float;
}
== add
{
   foo():Float ==
   {
      import from Float;

      -- The use of `f' before definition its is okay because it is
      -- a map and therefore lazy. However, the use of `pi' is not
      -- okay because it is a non-lazy constant that has not been
      -- defined yet and we aren't at the top-level of an `add'.
      p : Float == f(pi);

      pi: Float == 3.14159;
      f(a: Float): Float == {a*a};

      p;
   }


   -- The use of `g' and `qi' before they are defined is fine at the
   -- top-level of an `add' because the compiler sorts definitions
   -- so that they appear in the correct order.
   q : Float == f(qi);

   qi: Float == 3.14159;
   g(a: Float): Float == {a*a};
}


import from Float, Foo;
print << "pi**2 = " << foo() << newline;
