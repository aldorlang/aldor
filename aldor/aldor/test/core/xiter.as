#include "foamlib"
#pile

XGenerator(T: with): with == add

import from MachineInteger

qq(): (MachineInteger, MachineInteger) == (1,2)

listIter(l: List MachineInteger): Generator MachineInteger == generate
    t := l
    while not empty? t repeat
        yield first t
	t := rest t

foo(): Generator MachineInteger ==
   generate
       yield 1
       yield 1

bar(): MachineInteger ==
   total: MachineInteger := 0
   for a in foo() repeat
       total := total + a
   return total

listTotal(l: List MachineInteger): MachineInteger ==
    total: MachineInteger := 0
    for x in listIter(l) repeat total := total + x
    total

xfoo(): XGenerator MachineInteger ==
   xgenerate
       yield 1
       yield 1

xbar(): MachineInteger ==
   total: MachineInteger := 0
   xfor a in xfoo() repeat
       total := total + a
   return total

xlistIter(l: List MachineInteger): XGenerator MachineInteger == xgenerate
    t := l
    while not empty? t repeat
        yield first t
	t := rest t

xlistTotal(l: List MachineInteger): MachineInteger ==
    total: MachineInteger := 0
    xfor x in xlistIter(l) repeat total := total + x
    total

