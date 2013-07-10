-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

import from Integer


f( a: Integer, b: Integer == 10, c: Integer , d: Integer ):() == 
	print << a

#if TestErrorsToo

f( d==false, c==3, a==1 ) -- Missing par. 1

#endif

f( d==4, c==3, b==2, a==1 ) -- correct

f( d==4, c==3, a==1 ) -- correct

---------------------------------------

g( a: Integer, b: Integer, c: Integer , d: Integer ):() == 
	print << a


g( 1, c==1, b==1, 1) -- correct (BUG: positional-passing after by-name-passing)

#if TestErrorsToo

g( 1, d==1, b==3, 1 )  -- Par. missing c:Integer

g( d==1, 1, 2, 3 )     -- Par. missing a:Integer

#endif

---------------------------------------

h( a: Integer==1, b: Integer, c: Integer , d: Integer ):() == 
	print << a

h( d==1,  2, 3) -- correct

#if TestErrorsToo

h( d==1, 2 )	-- Par. missing c:Integer

#endif


----------------------------------------

C0 : Category == with
		i : Integer -> Integer

local l: (Integer, __:Integer == 2, __: C0 , y: Integer == 4) -> Integer

l(a: Integer, b: Integer == 2, x: C0, y: Integer == 4): Integer ==
	a

#if TestErrorsToo

i1: Integer == l(1, x: Integer == 3) -- Bad arg type; "should use x==3 ...
i2: Integer == l(1, 2, x: Integer == 3) -- Bad arg type; no "should ...

#endif

