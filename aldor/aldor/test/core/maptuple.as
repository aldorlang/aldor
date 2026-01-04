#include "foamlib"
#pile

MapMangler(a: Tuple Type, f: a -> SingleInteger): with
   skr: a -> SingleInteger
== add
   skr(s: a): SingleInteger == f s

import from SingleInteger;

foo(): () == 
   import from MapMangler( (String, SingleInteger), (a: String, b: SingleInteger): SingleInteger +-> b)
   import from String
   skr("hello", 22)


foo()
