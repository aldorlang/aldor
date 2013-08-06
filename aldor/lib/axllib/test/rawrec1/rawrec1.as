-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -Q3 -l axllib

#include "axllib"
 
main():() ==
{
   local rec:RawRecord(x:SingleInteger, y:DoubleFloat);

   rec := [42, 0.135];
   print << "rec.x = " << (rec.x) << newline;
   print << "rec.y = " << (rec.y) << newline;
   dispose! rec;
}

main();
