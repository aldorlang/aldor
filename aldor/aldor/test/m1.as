#include "foamlib"
#pile
#library M "m0.ao"
import from M

Foo(T: BasicType): with
    eq: T -> T -> PPartial T
    export from T
== add
    eq(x: T)(y: T): PPartial(T) == never

test3(n: MachineInteger): Boolean ==
    import from Foo MachineInteger
    12 case eq(n)(?)
