#library L0 "l0.ao"
#pile
import from L0
import from TT

--f(x: ?): () == consume x

foo(): () ==
    g == (): ? +-> value
    g()
    never
