#include "foamlib"
#pile

KerCat(S: with): Category == with
KerCat1(S1: with): Category == with KerCat S1

Ker(T: with): KerCat1 T == add

KerP: KerCat % with == Ker % add

-- ExpressionSpace2(K : KernelCategory(%)) : Category == with
ExprSpace2K(K: with KerCat(K)): Category == with
ExprSpace2P(K: with KerCat(%)): Category == with
