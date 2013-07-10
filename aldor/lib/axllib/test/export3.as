-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib"

C1: Category == with
		=:(%, %) -> %

		default	(a:%) = (y:%):% == a


C2: Category == with
		pietro: (%, %, %) -> Integer

Y: C1 == add
	(x:%) = (y:%): % == x

Z: C2 == add
	pietro(x:%,y:%,z:%): Integer == 1

#if TestErrorsToo

X: Join(C1, C2) with
		+: (%, %) -> %
		-: (%, %) -> %
		*: (%, %) -> %
		>: (%, %) -> %
    		nil: Integer
   == Y add
      (x:%) + (y:%): % == x
      (x:%) - (y:%): % == x

#endif
