#include "foamlib"
#pile
#library M "m0.ao"
import from M
#library ML "matchlist.ao"
import from ML

import from MachineInteger
import from LList MachineInteger

Foo(T: BasicType): with
    eq: T -> T -> PPartial T
== add
    eq(x: T)(y: T): PPartial(T) == never

test3(): Boolean ==
    import from Foo MachineInteger
    12 case eq(1)(?)



