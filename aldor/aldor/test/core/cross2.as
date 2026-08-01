#include "foamlib"
#include "assertlib"
#pile

Sym: with == add
Expr: with == add

L(T: with): with
    bracket: Tuple T -> %
== add
    [a: Tuple T]: % == never

Foo: with
== add
    Subst == Cross(Sym, Expr)
    Rep == L Subst
    import from Rep

    one(s: Sym, e: Expr): Rep == [ (s, e)@Subst ]
    