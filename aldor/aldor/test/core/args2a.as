#include "foamlib"
#pile

Compble: Category == with
    >: (%, %) -> Boolean
OSet(A: with): Category == with
    contains?: (%, A) -> Boolean

-- KernelCategory(S : Comparable) : Category == Join(OrderedSet, Patternable S)
KerCat(S: Compble): Category == with
    OSet S

Ker(S: Compble): KerCat S with Compble == add
    contains?(a: %, s: S): Boolean == never
    (>)(a: %, b: %): Boolean == never

-- ExpressionSpace2(K : KernelCategory(%)) : Category == with Comparable
KC ==> with Join(KerCat %, Compble)
ExprSpace2(K: KC): Category == with Compble
