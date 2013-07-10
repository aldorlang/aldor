-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib"

C1: Category == with
		=:(%, %) -> %

C2: Category == with
		pietro: (%, %, %) -> Integer

#if TestErrorsToo

X: Join(C1, C2) with
		+: (%, %) -> %
		-: (%, %) -> %
		*: (%, %) -> %
		>: (%, %) -> %
    		nil: Integer
   == add
      (x:%) + (y:%): % == x
      (x:%) - (y:%): % == x

#endif
