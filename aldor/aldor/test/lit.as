#include "foamlib"
#include "assertlib"
#pile

import from MachineInteger

foo()(n: MachineInteger): MachineInteger == n

bar(): MachineInteger == foo()(2)

total(l: List MachineInteger): MachineInteger ==
    tt := 0
    ll := l
    while not empty? ll repeat
        tt := tt + first ll
    	ll := rest ll
    return tt

import from Assert MachineInteger
import from List MachineInteger
assertEquals(2, bar())
assertEquals(5, foo()(5))
assertEquals(6, total [1,2,3])
