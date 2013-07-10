
#include "axllib"
#include "debuglib"

SI ==> SingleInteger;

import from NewDebugPackage;
#if AUTO
trace!(steps? == true);
-- trace!();
#else
start!();
#endif


FooDomain:BasicType with
{
   foo: SI -> %;
   wow: % -> %;
   bar: % -> SI;
}
== add
{
   Rep == SI;
   import from Rep;

   foo(x:SI):% == per x;
   bar(x:%):SI == rep x;
   wow(x:%):% ==
   {
      local result:% := foo(bar(x) + 1);
      return result;
   }

   sample:% == foo 0;

   (x:%) = (y:%):Boolean ==
   {
      debugging? := not stopped?();
      if (debugging?) then stop!();
      local result:Boolean := (bar x) = (bar y);
      if (debugging?) then start!();
      result;
   }

   (tw:TextWriter) << (x:%):TextWriter ==
   {
      pause!();
      tw << (bar x);
      unpause!();
      tw;
   }
}


main():() ==
{
   import from SI;
   import from FooDomain;

   print << (bar wow wow wow foo 23) << newline;
   print << (foo 23) << newline;
}

main();
