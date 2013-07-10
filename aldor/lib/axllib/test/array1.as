-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#include "axllib.as"
#pile
SI ==> SingleInteger
import from Integer
f(i:SI,n:SI):Array(SI) ==
  array((if j=i then 1 else 0) for j in 1..n)




