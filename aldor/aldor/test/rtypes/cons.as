#library L0 "l0.ao"
#pile
import from L0
import from TT
import from Boolean

macro {
	rep x == x @ % pretend Rep;
	per r == r @ Rep pretend %;
}

Cons: with
    --cons: (SExpression2, SExpression2) -> %
    first: % -> SExpression2
    rest: % -> SExpression2
    setFirst!: (%, SExpression2) -> ()
    --setRest!: (%, SExpression2) -> ()
    =: (%, %) -> Boolean
== add 
    Rep == Record(car: SExpression2, cdr: SExpression2)
    import from Rep, SExpression2
    
    first(cons: ?): ? == rep(cons).car
    rest(cons: ?): ? == rep(cons).cdr
    --cons(car: ?, cdr: ?): % == per [car, cdr]


    setFirst!(cons: ?, sx: ?): () == rep(cons).car := sx
    --setRest!(cons: %, sx: SExpression2): () == rep(cons).cdr := sx
    
    (cons1: ?) = (cons2: ?): Boolean ==
        first cons1 = first cons2 and rest cons1 = rest cons2

SExpression2: with
    =: (%, %) -> Boolean
== add
    (a: %) = (b: %): Boolean == never
    
