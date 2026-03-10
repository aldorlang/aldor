#include "foamlib"
#pile

define KerCat(S: with): Category == with {};
define Expr2Cat(K: with KerCat(%)): Category == with {};

Foo: KerCat(%) with == add

AA: Expr2Cat(Foo) with == add

--KX: KerCat(KX) with == add

--E2: Expr2Cat(KX) == add

#if WorkAround
-- Expr2(K: with): Category == PrimitiveType with {
Expr3(K: with): Category == with {
        if K has KerCat(%) then AdditiveType;
}
#else
#endif

