#include "foamlib"
#include "assertlib"
#pile

import from MachineInteger
import from List MachineInteger
import from Partial MachineInteger

listIter(l: List MachineInteger): Generator MachineInteger == generate
    local tt := l
    local done := false
    while not empty? tt repeat
        yield first tt
	tt := rest tt

listTotal(l: List MachineInteger): MachineInteger ==
    total: MachineInteger := 0
    for x in listIter(l) repeat total := total + x
    total

import from Assert MachineInteger
assertEquals(0, listTotal [])
assertEquals(10, listTotal [1,2,3,4])
