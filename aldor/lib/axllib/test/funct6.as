-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib"
#pile

SI ==> SingleInteger

import from SI
import from Boolean
import from Integer

L: with 
	import from Float
	f : () -> Float;
== add
	import from Float
	f ():Float == 3.1


H: with
	import from SI
	import from Boolean

	id  : SI -> SI
	foo : (%, %, Integer) -> SI
	foo : (%, %, SI) -> SI
	foo : (%, %, Boolean) -> SI
	foo : (%, %, Boolean) -> %
  == add
		import from SI
		import from Boolean

		id ( x: SI ):SI == x
		foo ( x:%, y:%, u:Integer ):SI == 1
		foo ( x:%, y:%, u:SI ):SI == 2
		foo ( x:%, y:%, u:Boolean ):SI == 34
		foo ( x:%, y:%, u:Boolean ):% == x

import
	float: Literal -> %
from Float

import from L

mantissa(i:SI):Integer == 1

a: Integer == mantissa( 1.3 )

b: Integer == exponent( 1.2 )

