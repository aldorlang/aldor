-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib"

main(quickie?:Boolean):SingleInteger ==
{
   quickie? => return; -- Return without a value ...
   42;
}


import from SingleInteger;
print << main(true) << newline;

