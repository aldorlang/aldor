-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -W Td+scobind -l axllib
#pile

-- This file tests that nullary constructors can be defined and used.

#include "axllib.as"

F ==> Float;

SpeedTestFloat():with
    libTest: (SingleInteger) -> F
      ++ libTest(n) runs a timing loop n times and returns
      ++ a final value.

  == add

    libTest(n:SingleInteger): F ==
       local a, b, x : F
       local i: SingleInteger:=1
       (a, b, x) := (0.99, 0.0001, 0.1)
       repeat
         if i>n then return x
         i:=i+1
         x:=(a*x+b)
       x


local f: () -> F

f(): F ==
    import from
        SingleInteger
        TextWriter
        SpeedTestFloat()
    r : F := libTest(10)
    print<<r<<newline
    r

f()
