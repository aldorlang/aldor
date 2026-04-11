#include "foamlib"
#pile
#library M "m0.ao"
import from M

LList(X: with): with
    cons: (X, %) -> %
    nil: () -> %
    nil?: % -> Boolean

    first: % -> X
    rest: % -> %

    cons: % -> PPartial(X, %)
    isNil: % -> PPartial()

    bracket: MachineInteger -> % -> PPartial Tuple X
== add
    Rep == Record(elt: X, r: %)
    import from Pointer, Rep

    first(l: %): X == rep(l).elt
    rest(l: %): % == rep(l).r

    cons(t: X, r: %): % == per [t, r]
    nil(): % == nil()$Pointer pretend %
    nil?(l: %): Boolean == nil?(l pretend Pointer)

    cons(l: %): PPartial(X, %) ==
        nil? l => failed()
	success(first l, rest l)

    isNil(l: %): PPartial() ==
        if nil? l then success() else failed()

    bracket(n: MachineInteger)(l: %): PPartial Tuple X == never

#if 0

    braket: TupleShape -> % -> PPartial Tuple X

...
    FullMatch(S, A, B): with
        shape: () -> S

    Shape: with
        argCount: % -> MachineInteger
	isWildcard: (%, MachineInteger) -> %

    bracket: (s: Shape, %) -> PPartial T

Foo: with
    plus: FullMatch( (%, %), %)
== add
    plus(a: %, b: %): % == per(rep(a) + rep(b))
    plus(a: %)(r: %): PPartial % == success(rep(r) - rep(a))
    plus(a: %)(r: %): Generator PPartial(%, %) == success(rep(r) - rep(a))
#endif    
#if 0
Eq(T: BasicType): with
    eq: T -> PatMatch(T, T)
== add
    eq(x: T): PatMatch(T, T) == never
#endif

