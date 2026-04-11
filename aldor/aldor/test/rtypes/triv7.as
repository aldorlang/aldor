#library L0 "l0.ao"
#pile
import from L0
import from TT

--f(x: ?): () == consume x
-- Test binary functions

foo(): () ==
    g == (a: ?, b:?): ? +-> a*b
    g(value, value)
    never
