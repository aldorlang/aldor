#include "foamlib"
#pile

-- This ought to work but fails in genfoam due to Rep being recursive
XRec: with 
    =: (%, %) -> Boolean
== add 
    Rep == Record(x: MachineInteger, p: Rep)
    import from Rep
    (a: %) = (b: %): Boolean == rep(a).x = rep(b).x


