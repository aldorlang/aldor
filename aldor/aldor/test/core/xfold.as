#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

#library X "x.ao"
import from X
import from SInt

XFold(T: with): with
	 /: (f: (T,T) -> T, XGenerator T) -> T
== add
  Rep ==> (T, T) -> T

  (f: (T, T) -> T) / (g: XGenerator T): T == 
     first: Boolean := true
     local acc: T
     xfor elt in g repeat 
         acc := if first then elt else f(acc, elt)
	 first := false
     first => never
     return acc

range(n: SInt): XGenerator SInt == xgenerate for x in 1..n repeat yield x

import from Assert SInt
import from XFold SInt
foldSum(n: SInt): SInt == (+)/(range n)

testSum(): () ==
    xfor n in range(10) repeat
        assertEquals(n * (n+1) quo 2, (+)/(range n))

testSum()
