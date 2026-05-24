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

-- Test functions with 2 arguments
ADT1: with
    create: () -> %
    f: % -> %
== add
    Rep == Record()
    import from Rep

    create(): ? == per []
    f(a: ?): ? == a
    

