#include "foamlib"
#pile
#library M "m0.ao"
import from M

--L: with { a: () -> Record % } == add { a(): Record % == never }
--f(x: L): () == a()

L: with { a: () -> PPartial %; b: () -> Record % } == add { a(): PPartial % == never; b(): Record % == never }

f(x: L): () == { b(); a() }
#if 0
XList: with
    tst: MachineInteger -> PPartial(%)
== add
    tst(n: MachineInteger): PPartial(%) == failed()

test1(): () ==
    import from XList, MachineInteger
    tst(22)
#endif
#if 0
LList(X: with): with
    --cons: % -> PPartial(X, %)
    cons1: % -> PPartial(%)
    --cons2: % -> Cross(PPartial(X, %), Record(X, %))
    --cons3: % -> PPartial(X, X)
    --cons4: % -> PPartial(%, X)
    cons5: % -> Cross(%)
    --foo: % -> Record(X, %)
== add

    Rep == Record(elt: X, r: %)
    import from Pointer, Rep

    local nil?(l: %): Boolean == nil?(l pretend Pointer)

    --cons(l: %): PPartial(X, %) == failed()
    cons1(l: %): PPartial(%) == failed()
    --cons2(l: %): Cross(PPartial(X, %), Record(X, %)) == never
    --cons3(l: %): PPartial(X, X) == failed()
    --cons4(l: %): PPartial(%, X) == failed()
    cons5(l: %): Cross(%) == never
    --foo(l: %): Record(X, %) == never

test2(l1: LList MachineInteger): Boolean ==
    --cons(l1)
    cons1(l1)
    --cons3(l1)
    --cons4(l1)
    --cons5(l1)
    --foo(l1)
    false
#endif
