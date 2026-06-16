#library L0 "l0.ao"
#library LL "ll.ao"
#pile
import from L0, LL
import from TT
import from Boolean

macro {
	rep x == x @ % pretend Rep;
	per r == r @ Rep pretend %;
}

SX: with == add

-- Test functions with 2 arguments
ADT2: with
    --create: SX -> %
    --f: % -> SX
    f!: (%, SX) -> ()
== add
    Rep == Record(m: SX)
    import from Rep

    --create(a: ?): ? == per [a]
    --f(a: ?): ? == rep(a).m
    f!(a: %, sx: ?): () == rep(a).m := sx

