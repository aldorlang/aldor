#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean

foo2(): ? ==
    import from Record(f: Boolean)
    qq(x: ?): ? ==
        x.f := true
 	x
    qq [false]