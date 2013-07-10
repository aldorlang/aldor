-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -Q3 -l axllib

#include "axllib"

Dense ==> Join(DenseStorageCategory, BasicType);


make(T:Dense, x1:T, x2:T):() ==
{
   local rec:RawRecord(lo:T, hi:T);
   rec := [x1, x2];

   print << "rec.lo = " << (rec.lo) << newline;
   print << "rec.hi = " << (rec.hi) << newline;
}


import from SingleInteger;
make(SingleInteger, 42, 21);

import from DoubleFloat;
make(DoubleFloat, 4.2, 2.1);
