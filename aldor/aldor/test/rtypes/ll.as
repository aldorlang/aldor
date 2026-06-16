#library L0 "l0.ao"
#pile

import from L0

TL(X: with): with
    cons: (X, %) -> %
    nil: %
    first: % -> X
    rest: % -> %
    nil?: % -> Boolean
== add
    nil: % == never
    cons(a: X, b: %): % == never
    first(x: %): X == never
    rest(x: %): % == never
    nil?(x: %): Boolean == never
