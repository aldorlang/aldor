#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

import from List List SInt
import from List List SInt
import from List SInt
import from Assert List List SInt
import from Assert List SInt
import from Assert SInt
import from SInt

foo(l: List SInt): List List SInt == [x for x in tails l]

assertEquals(cons(12, cons(11, cons(10, empty()))), [12, 11, 10])
assertEquals(12, first [12, 11, 10])
assertEquals([11,10], rest [12, 11, 10])

assertEquals(2, # [tails [1,2]])