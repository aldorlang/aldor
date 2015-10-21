#include "foamlib"
#pile

F: with
    apply: (%, %) -> %
    f: %
    g: %
== add
    Rep == MachineInteger
    import from Rep

    apply(a: %, b: %): % == per(rep a+rep b)
    g: % == per 1
    f: % == per 2
    
test(): () ==
    import from F
    f g;
