-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

IB ==> (Integer, Boolean)

(a:  Integer ) =  (b: Integer): IB     ==               (b,       a = b)
(ac: Cross IB) =  (b: Integer): IB     == ((a,c) := ac; (b, c and a = b))
(ac: Cross IB) =  (b: Integer): Boolean== ((a,c) := ac; (   c and a = b))

(a:  Integer ) ~= (b: Integer): IB     ==               (b,       a ~= b)
(ac: Cross IB) ~= (b: Integer): IB     == ((a,c) := ac; (b, c and a ~= b))
(ac: Cross IB) ~= (b: Integer): Boolean== ((a,c) := ac; (   c and a ~= b))

(a:  Integer ) >  (b: Integer): IB     ==               (b,       a > b)
(ac: Cross IB) >  (b: Integer): IB     == ((a,c) := ac; (b, c and a > b))
(ac: Cross IB) >  (b: Integer): Boolean== ((a,c) := ac; (   c and a > b))

(a:  Integer ) <  (b: Integer): IB     ==               (b,       a < b)
(ac: Cross IB) <  (b: Integer): IB     == ((a,c) := ac; (b, c and a < b))
(ac: Cross IB) <  (b: Integer): Boolean== ((a,c) := ac; (   c and a < b))

(a:  Integer ) >= (b: Integer): IB     ==               (b,       a >= b)
(ac: Cross IB) >= (b: Integer): IB     == ((a,c) := ac; (b, c and a >= b))
(ac: Cross IB) >= (b: Integer): Boolean== ((a,c) := ac; (   c and a >= b))

(a:  Integer ) <= (b: Integer): IB     ==               (b,       a <= b)
(ac: Cross IB) <= (b: Integer): IB     == ((a,c) := ac; (b, c and a <= b))
(ac: Cross IB) <= (b: Integer): Boolean== ((a,c) := ac; (   c and a <= b))


import from String
import from Integer

foo(): () ==
	print<<(if 0 < 2 <= 2 > 1 ~= 5 = 5 >= 4 then "OK" else "Not")<<newline
	print<<(if 0 < 0 <= 2 > 1 ~= 5 = 5 >= 4 then "OK" else "Not")<<newline

foo()
