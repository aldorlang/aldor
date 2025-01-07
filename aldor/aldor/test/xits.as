#include "foamlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger

doubles(g: XGenerator MachineInteger): XGenerator MachineInteger == xgenerate
    xfor i in g repeat
        yield 2 * i

range(n: MachineInteger, m: MachineInteger): XGenerator MachineInteger == xgenerate
    local i := n
    while i < m repeat
        yield i
	i := i + 1

sum(g: XGenerator MachineInteger): MachineInteger == 	
    total: MachineInteger := 0
    xfor x in g repeat
        total := total + x
    total

sumN(m: MachineInteger): MachineInteger == sum(range(0, m))

sumD(m: MachineInteger): MachineInteger == sum(doubles(range(0, m)))

print << "Sum 4: " << sumN(4) << newline
