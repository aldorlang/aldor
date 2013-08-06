-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testrun -l axllib
--> testerrs


#include "axllib"


smallRadixTest():() ==
{
   import from SingleInteger;

   print << "Start with some small numbers ..." << newline;
   print << "1000 base 2 =  " << (2r1000) << " (ought to be  8)" << newline;
   print << "F base 16   = " << (16rF)    << " (ought to be 15)" << newline;
   print << "Z base 36   = " << (36rZ)    << " (ought to be 35)" << newline;
}


bigRadixTest():() ==
{
   import from Integer;

   print << newline << "Now some really big numbers ..." << newline;

   -- A binary number that won't fit into an immediate
   print << "110011101011100111110001111010111000111 base  2 = "
         << 2r110011101011100111110001111010111000111 << newline;


   -- A silly radix 36 number ...
   print << "                     MARYHADALITTLELAMB base 36 = "
         << 36rMARYHADALITTLELAMB << newline;
}


-- Check for some radix integers
smallRadixTest();
bigRadixTest();


-- Check for errors ...
#if TestErrorsToo
import from SingleInteger;

print << "1 base 37   = " << (37rZ) << " (is invalid)" << newline;
print << "1 base 1    = " << (1rZ)  << " (is invalid)" << newline;
#endif

