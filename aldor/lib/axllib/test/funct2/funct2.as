-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib.as"

import from Integer

H(n: Integer): with {zero: ()->%} == add { zero():%== 0 pretend %;}

foo (a: Integer) : H a == { import from H a; zero();}

#if TestErrorsToo

z : Boolean == false
z : Integer == 1

y : Integer == 1

x : H 1 == foo( y )

#endif



