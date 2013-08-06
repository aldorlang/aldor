-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Cut-down version of bug 1090.

#include "axllib"

-- Don't optimise or the problem will vanish into the bit bucket!
--> testrun -laxllib -Q0


-- This file detects domain initialisation bugs in the def-group
-- analysis. Previously, when any domain export was required we
-- initialised the exports and locals in the order Rep, foo, boom,
-- bar and trouble. However, because the only reference to the
-- test() function we didn't import test$List Bar() until after
-- bar was given its value. Since bar calls test we segfault.
--
-- The fix is to ensure that all maps, local or exported, are
-- initialised before non-map exports.
--
-- Note that this problem only seems to arise when the list type
-- (Bar() in the example below) is sufficiently complicated. This
-- may mean that it only applies to dependent types.
define BarCat(S:AbelianMonoid):Category == with
{
   bob: () -> %;
}

Bar(dim:SingleInteger, S:AbelianMonoid):BarCat(S) == add
{
   Rep == S;
   import from Rep;

   bob():% == per 0;
}


Foo(num:SingleInteger, argList:List Bar(num, SingleInteger)):with
{
   bar:    %;
   foo:    SingleInteger -> %;
   boom:   () -> ();
}
== add
{
   Rep == SingleInteger;
   import from Rep;

   bar:% == foo(0$SingleInteger);

   local trouble():() == test argList;

   foo(x:SingleInteger):% ==
   {
      trouble();
      print << "Success!" << newline;
      per x;
   }

   boom():() == {}
}


main():() ==
{
   import from SingleInteger;
   import from List Bar(3, SingleInteger);
   import from Foo(3, [bob(), bob(), bob()]);
   boom();
}


main();

