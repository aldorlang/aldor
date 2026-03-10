#include "foamlib"
#include "assertlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger
import from Partial MachineInteger

xfoo(): XGenerator MachineInteger ==
   xgenerate
       yield 1
       yield 2

xbar(): MachineInteger ==
   total: MachineInteger := 0
   xfor a in xfoo() repeat
       total := total + a
   return total

print << "yield-2 total " << xbar() << newline

xlistIter(l: List MachineInteger): XGenerator MachineInteger == xgenerate
    local tt := l
    local done := false
    while not empty? tt repeat
        yield first tt
	tt := rest tt

xlistTotal(l: List MachineInteger): MachineInteger ==
    total: MachineInteger := 0
    xfor x in xlistIter(l) repeat total := total + x
    total

print << "list total " << xlistTotal([1,2,3,4]) << newline

xfoo2(): MachineInteger ==
    for x in 1..10 repeat
	a := x
    return a

import from Assert MachineInteger
assertEquals(10, xlistTotal [1,2,3,4])

