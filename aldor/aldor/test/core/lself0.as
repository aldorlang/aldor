#include "foamlib"
#pile

XLeftModule(R: with): Category == with
    *: (R, %) -> %

XRightModule(R: with): Category == with
    *: (%, R) -> %

XBiModule(R: with): Category == Join(XLeftModule R, XRightModule R) with

XGroup: Category == XBiModule % with
    1: %

Abel(R: with): Category == Join(XGroup, XBiModule R) with

SUPC(G: with): Category == Abel(G) with

SUP(R: with): SUPC(R) with
== add
    (*)(a: %, b: %): % == never
    (*)(r: R, b: %): % == never
    (*)(r: %, b: R): % == never
    1: % == never

Foo0: XGroup with
    foo: SUP % -> %
== add
    1: % == never
    (a: %) * (b: %): % == never
    foo(x: SUP %): % ==
        c: % := 1
	c * x
	never
