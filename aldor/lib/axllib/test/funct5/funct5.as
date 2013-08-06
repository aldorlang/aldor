-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib"
#pile

SI ==> SingleInteger

import from SI
import from Boolean
import from Integer

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

foo ( a: SI, b: SI ): SI == a
foo ( a: Boolean, b: SI, c:SI ):Boolean == a

local x:*H;

d:*H == foo( x,x,false) -- error

e:*H == foo( x,x,false) -- error

a:SI == id(1)  -- error

