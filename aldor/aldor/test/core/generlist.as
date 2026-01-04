#include "foamlib"
#include "assertlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger
import from Partial MachineInteger

xlistIter(ll: List MachineInteger): XGenerator MachineInteger == xgenerate
    local tt := ll
    local done := false
    while not empty? tt repeat
        yield first tt
	tt := rest tt

xlistTotal(l: List MachineInteger): MachineInteger ==
    total: MachineInteger := 0
    xfor x in xlistIter(l) repeat total := total + x
    total

import from Assert MachineInteger
assertEquals(0, xlistTotal [])
assertEquals(10, xlistTotal [1,2,3,4])
