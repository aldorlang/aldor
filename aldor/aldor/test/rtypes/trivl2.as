#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean
import from TL TT

foo2(): () ==
    g == (a: ?): ? +-> if nil? a then value else first a * g rest a
    g(nil)
    never
