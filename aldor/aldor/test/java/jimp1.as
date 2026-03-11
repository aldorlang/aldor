#include "foamlib"
#pile

import from Machine;

APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

import
    System: with
        currentTimeMillis: () -> Long
    Long: with
from Foreign Java "java.lang"

foo(): () ==
    import from System
    l := currentTimeMillis()
