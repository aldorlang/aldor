-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
--> testrun -laxllib
--> testrun -O -laxllib

#include "axllib"

main(quickie?:Boolean):() ==
{
   quickie? => return; -- Return without a value okay
   print << "Slow!" << newline;
   return true; --------- Return with value generates warning.
}


main(true);

