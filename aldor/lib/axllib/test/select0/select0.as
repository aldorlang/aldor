-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -laxllib
--> testrun -O -laxllib

#include "axllib"

main(a:SingleInteger):() ==
{
   select a in
   {
      0 => print << "Zero." << newline;
      1 => print << "One." << newline;
      print << "Ooops" << newline;
   }

   print << ">> Done <<" << newline;
}


import from SingleInteger;
main(0);
main(1);
main(42);

