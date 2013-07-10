
#include "axllib"
#include "debuglib"

SI ==> SingleInteger;

start!()$NewDebugPackage;

FooDomain:with
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
   wow(x:%):% == foo(bar(x) + 1);
}


wibble():() ==
{
   main();
}

main():() ==
{
   import from SI;
   import from FooDomain;

   print << (bar wow wow wow foo 23) << newline;
}


main();
wibble();
