-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testcomp
--> testrun -laxllib

#include "axllib"

SI ==> SingleInteger;

define MyCross == Cross(SI, SI);


foo(x:Cross(SI, SI)):() ==
{
   print << "Yo!" << newline;
   (a, b) := x;
   print << "(" << a << ", " << b << ")" << newline;
}

import from SI, MyCross;

local xy:MyCross := (2, 6);
foo(xy);

