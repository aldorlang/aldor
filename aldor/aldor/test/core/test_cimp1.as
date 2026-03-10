#include "foamlib"
#pile

import { Value: CPointer } from Foreign C("valtest.h")

import
    valueNew: MachineInteger -> Value
    valueFree: Value -> ()
    valueIncrement: (Value, Value) -> ()
    valueAsInt: Value -> MachineInteger
from Foreign C("valtest.h")

test(): () ==
    v1: Value = valueNew(22)
    v2: Value = valueNew(22)
    valueIncrement(v1, v2)
    stdout << valueAsInt v1 << newline
