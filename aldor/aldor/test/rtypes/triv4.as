#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean

foo2(): () ==
    g == (x: ?): ? +-> if true then x else value
    g(value)
    never
