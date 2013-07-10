
#include "axllib"
#include "debuglib"

SI ==> SingleInteger;

start!()$NewDebugPackage;

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
      -- Hide our internal calls
      pause!()$NewDebugPackage;
      local result:% := foo(bar(x) + 1);
      unpause!()$NewDebugPackage;
      return result;
      print << "Yo" << newline;
      return sample;
   }

   sample:% == foo sample;

   (x:%) = (y:%):Boolean ==
   {
      pause!()$NewDebugPackage;
      local result:Boolean := (bar x) = (bar y);
      unpause!()$NewDebugPackage;
      result;
   }

   (tw:TextWriter) << (x:%):TextWriter ==
   {
      pause!()$NewDebugPackage;
      tw << (bar x);
      unpause!()$NewDebugPackage;
      tw;
   }
}


main():() ==
{
   import from SI;
   import from FooDomain;

   print << (bar wow wow wow foo 23) << newline;
}

main();
