#include "foamlib"
#include "assertlib"
#pile

import from Assert MachineInteger
oneVoid(): Generator Cross() == generate yield ()

test(): () ==
    c: MachineInteger := 0
    for x in oneVoid() repeat c := c + 1
    assertEquals(1, c)

test()
