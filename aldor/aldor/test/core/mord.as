#include "foamlib"
#pile

define PO: Category == with
    <=: (%, %) -> Boolean
    >=: (%, %) -> Boolean
    default
        (a : %) >= (b : %): Boolean == b <= a

    toOrder: MachineInteger -> %

MyOrder: PO with
    toOrder: MachineInteger -> %
== add
    Rep ==> MachineInteger
    import from Rep

    (a : %) <= (b : %): Boolean == rep(a) <= rep(b)

    toOrder(n: MachineInteger): % == per n

