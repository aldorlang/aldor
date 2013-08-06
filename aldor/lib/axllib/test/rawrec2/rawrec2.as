-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -Q3 -l axllib

#include "axllib"

Dense ==> Join(DenseStorageCategory, BasicType);


make(T:Dense, x1:T, x2:T):RawRecord(lo:T, hi:T) == [x1, x2];

show(T:Dense, rec:RawRecord(lo:T, hi:T)):() ==
{
   print << "rec.lo = " << (rec.lo) << newline;
   print << "rec.hi = " << (rec.hi) << newline;
}


main():() ==
{
   import from SingleInteger;
   show(SingleInteger, make(SingleInteger, 42, 21));
}


main();
