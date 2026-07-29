#include "foamlib"
#pile

XRec: with 
    =: (%, %) -> Boolean
== add 
    Rep == Record(x: MachineInteger, p: Rep)
    import from Rep
    (a: %) = (b: %): Boolean == rep(a).x = rep(b).x


