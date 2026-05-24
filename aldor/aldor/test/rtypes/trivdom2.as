#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean

DD: with
    f: % -> %
    f: % -> TT
== add
    f(x: ?): ? == x
    f(x: ?): TT == create()



