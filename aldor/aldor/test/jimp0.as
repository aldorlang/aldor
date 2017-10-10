#include "foamlib"
#pile

import from Machine;

APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

import BitSet: with
     new: () -> %
     new: SingleInteger -> %
     APPLY(_and, % -> ())
from Foreign Java "java.util"

foo(): () ==
    import from SingleInteger
    bb: BitSet := new 22
    bb._and(bb)
