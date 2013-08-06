-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib"

#if TestErrorsToo

X: with
    +: (%, %) -> %
    -: (%, %) -> %
    *: (%, %) -> %
    >: (%, %) -> %
  == add
    (x:%) + (y:%): % == x
    (x:%) * (y:%): % == x
    (x:%) - (y:%): % == x

#endif
