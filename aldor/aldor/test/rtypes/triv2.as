#library L0 "l0.ao"
#pile
import from L0
import from TT

--f(x: ?): () == consume x

foo(): () ==
    g == (x: ?): TT +-> x
    g(value)
    never

--foo2(): () ==
--    g == (x: ?, y: ?): TT +-> if true then x else y
--    g(value)
--    never
