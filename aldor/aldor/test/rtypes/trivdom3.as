#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean

-- Test functions with 2 arguments
DD: with
    f: (%, %) -> %
    f: TT -> TT
== add
    f(x: ?, y: ?): % == x
    f(x: ?): ? == x

