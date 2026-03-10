#include "foamlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger

foo(n: MachineInteger, m: MachineInteger): XGenerator MachineInteger ==
  n := n  - 1
  xgenerate
    t: MachineInteger := 0
    while t < (n + m) repeat
        yield t
	t := t + 1

GTools(V: with, X: with, A: with): with
    gcompose: (X -> XGenerator A, V -> XGenerator X) -> V -> XGenerator A
    fcompose: (X -> A, V -> XGenerator X) -> V -> XGenerator A
    compose: (X -> A, V -> X) -> V -> A
== add

    compose(f: X -> A, g: V -> X)(v: V): A == f g v

    gcompose(f: X -> XGenerator A, g: V -> XGenerator X)(v: V): XGenerator A == xgenerate
        xfor x in g v repeat
            xfor a in f x repeat
	        yield a

    fcompose(f: X -> A, g: V -> XGenerator X)(v: V): XGenerator A == xgenerate
        xfor x in g v repeat
            yield f x

