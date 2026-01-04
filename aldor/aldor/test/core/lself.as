#include "foamlib"
#pile

#library LSELF0 "lself0.ao"
import from LSELF0

Foo: XGroup with
    foo: SUP % -> %
== add
    1: % == never
    (a: %) * (b: %): % == never
    foo(x: SUP %): % ==
        c: % := 1
	c * x
	never

