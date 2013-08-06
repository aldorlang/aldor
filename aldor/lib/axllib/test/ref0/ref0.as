-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib -Wno-keyword-ref

-- Test how well the compiler treats ref as a non-keyword

#include "axllib"


ref(x:SingleInteger):() ==
{
   (x = 0) => return;
   print << "x = " << x << newline;
   ref(x - 1);
}

import from SingleInteger;
ref(8);
