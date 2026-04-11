#library L0 "l0.ao"
#pile
import from L0
import from TT
import from Boolean

foo2(): () ==
    g == (x: ?, y: ?): TT +-> if true then x else y
    g(value, value)
    never
