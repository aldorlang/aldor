#include "foamlib"
#pile

import { Value: with } from Foreign C("valtest.h")

import from Machine
import
    valueNew: BSInt -> Value
    valueFree: Value -> ()
    valueIncrement: (Value, Value) -> ()
    valueAsInt: Value -> BSInt
    valueSet: (lhs: Value, rhs: Value) -> ()
from Foreign C("valtest.h")

test(): () ==
    import from MachineInteger
    v1: Value := valueNew(coerce 22)
    v2: Value := valueNew(coerce 1)
    valueIncrement(v1, v2)
    stdout << coerce valueAsInt v1 << newline
