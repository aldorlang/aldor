#include "types"
#include "aldorio"
#pile

-- Satisfier.. This needs a lot of extension to deal with cross/tuple
-- flummery, and should be combined with the unifier

-- cases:
-- tpossAdd..
--    add([int], (forall (x) x)) -> [(forall (x) x)]
--    .. justified by int sat Vx.x
--
-- tdn:
--   Vx.x is ok if wanting int (can unify)
SatResult: SExpressionOutputType with
    success: () -> %
    failed: () -> %
    result: Boolean -> %
    anyToNone: () -> %

    unifier: % -> UnifierPair

    success: UnifierPair -> %

    withUnifier: (%, UnifierPair) -> %

    test: % -> Boolean
== add
    Rep == Record(ret: MachineInteger, ur: UnifierPair)
    import from MachineInteger
    import from Unifier
    import from Rep
    import from Symbol
    
    failed(): % == per [0, failed()]
    success(): % == per [1, empty()]
    anyToNone(): % == per [2, empty()]

    success(u: UnifierPair): % == per [1, u]

    result(b: Boolean): % == if b then success() else failed()

    unifier(r: %): UnifierPair == rep(r).ur

    withUnifier(r: %, u: UnifierPair): % == per [rep(r).ret, u]

    test(n: %): Boolean == rep(n).ret > 0

    sexpression(r: %): SExpression ==
        if test r then sexpr(-"success") else sexpr(-"fail")

SatOptions: with
    none: () -> %
    allowUnify: () -> %
    combine: Tuple SatOptions -> %
== add
    Rep == MachineInteger
    import from Rep
    none(): % == per 0

    allowUnify(): % == per 1
    combine(t: Tuple SatOptions): % == never
    
TypeSatisfier: with
    satisfies?: (TypeTerm, TypeTerm, opts: SatOptions == none()) -> SatResult
    export from SatOptions
    export from SatResult
== add
    import from UnificationTools2
    import from Unifier
    import from UnifierPair

    local satCounter: CallCounter := counter("tfsat", false)

    satisfies?(s: TypeTerm, t: TypeTerm, options: SatOptions): SatResult ==
        cc := open satCounter
	writer(cc) << "(sat " << s << " " << t << newline
	r := satisfies1?(s, t)
	writer(cc) << " sat " << s << " " << t << " --> " << r << ")" << newline
	return r
	
    local satisfies1?(s: TypeTerm, t: TypeTerm): SatResult ==
	constant? s and constant? t => satisfiesConst(s, t)
	unknown? s => failed()
	unknown? t => success()
	type? t => error "type not implemented"
	exit? s => success()
	tuple? t => error "tuple not implemented"
	multi? t => error "multi not implemented"
	cross? t => error "cross not implemented"
	map? t => error "map not implemented"
	cross? s => error "left cross not implemented"
	tuple? s => error "left tuple not implemented"
	satGeneric(s, t)

    local satGeneric(s: TypeTerm, t: TypeTerm): SatResult ==
        s = t => success()
        ur := unify(s, t)
	failed? ur => failed()
	empty? ur => success()
	uS := apply(left ur, s)
	uT := apply(right ur, t)
	return withUnifier(satisfies1?(uS, uT), ur)
	
    local satisfiesConst(s: TypeTerm, t: TypeTerm): SatResult ==
	unknown? t => success()
	unknown? s => failed()
        s = t => success()
	failed()

    type?(t: TypeTerm): Boolean == false
    exit?(t: TypeTerm): Boolean == false
    tuple?(t: TypeTerm): Boolean == false
    multi?(t: TypeTerm): Boolean == false
    cross?(t: TypeTerm): Boolean == false
    map?(t: TypeTerm): Boolean == false

TestSatisfier: with
    testSat: () -> ()
== add
    import from Assert TypeTerm
    import from TypeSatisfier
    import from TypeTerm
    
    testSat(): () ==
        testSat1()
        testSat2()
        testSat3()

    local testSat1(): () ==
        tt1: TypeTerm := fromString "int"
	tt2: TypeTerm := fromString "(forall (x) x)"
	assertTrue(test satisfies?(fromString "int", fromString "(forall (x) x)"))
	assertTrue(test satisfies?(fromString "(forall (x) x)", fromString "int"))

	tt1 := fromString "(forall (x) (apply plus x one))"
	tt2 := fromString "(forall (a b) (apply plus a b))"
	--assertTrue(test satisfies?(tt1, tt2))
	--assertFalse(test satisfies?(tt2, tt1))

    local testSat2(): () ==
        stdout << "SAT2 " << newline
        assertTrue(test satisfies?(fromString "(forall (x) (apply list x))", fromString "(apply list int)"))
        stdout << "SAT2 DONE" << newline

    local testSat3(): () ==
        stdout << "SAT3" << newline
        tt1: TypeTerm := fromString "(forall (x) (apply map (comma x x) int))"
        tt2: TypeTerm := fromString "(forall (y) (apply map (comma y y) int))"
	sr := satisfies?(tt1, tt2)
	stdout << sr << newline
        stdout << "SAT3 DONE" << newline
	
#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestSatisfier
--testSat()
stdout << "DONE!" << newline
#endif
