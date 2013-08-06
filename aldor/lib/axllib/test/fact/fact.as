-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen c
#pile

-- Include a file with the basic library import
-- statements and macros.

#include "axllib.as"

-- State that the factorial operation is usable
-- outside this file.  The operation takes a
-- single argument of type Integer and returns
-- an object of that same type.

export factorial: Integer -> Integer

+++ factorial n computes the factorial of the
+++ object n of type Integer via a recursive
+++ algorithm.

factorial(n: Integer): Integer ==
        if n < 1 then 1
        else n * factorial(n - 1)
