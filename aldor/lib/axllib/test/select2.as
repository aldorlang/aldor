-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib"

#if TestErrorsToo
main(a:SingleInteger):() ==
{
   -- Only allowed general statements after all the case labels ...
   select a in
   {
      print << "(Testing for 0 ...)" << newline;
      0 => print << "Zero." << newline;

      print << "(Testing for 1 ...)" << newline;
      1 => print << "One." << newline;

      print << "Ooops" << newline;
   }

   print << ">> Done <<" << newline;
}


import from SingleInteger;
main(0);
main(1);
main(42);
#endif

