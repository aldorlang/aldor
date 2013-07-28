-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axldem -l axllib 
#pile

#include "axllib.as"

#library DemoLib       "axldem"
import from DemoLib

import from Integer, IntegerRoots, FormattedOutput

print."approxSqrt ~a is ~a~n"(<<88,<<approxSqrt 88)
print."approxSqrt ~a is ~a~n"(<<45678,<<approxSqrt 45678)
print."~n"
print."approxNthRoot(~a,~a) is ~a~n"(<<45678,<<3,<<approxNthRoot(45678,3))
print."approxNthRoot(~a,~a) is ~a~n"(<<4567, <<3,<<approxNthRoot(4567,3))
