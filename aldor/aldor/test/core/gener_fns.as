#include "foamlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger
import from Partial MachineInteger

threefns(n: MachineInteger): XGenerator(MachineInteger -> MachineInteger) == xgenerate
    k: MachineInteger == 8 * n
    yield (a: MachineInteger): MachineInteger +-> a + n + k
    yield (a: MachineInteger): MachineInteger +-> a + 2*n
    yield (a: MachineInteger): MachineInteger +-> a + 3*n

eatfns(consume: MachineInteger -> ()): () ==
    xfor f in threefns 12 repeat
        consume f 5
