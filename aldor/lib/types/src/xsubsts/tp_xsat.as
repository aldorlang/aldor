#include "types"
#include "aldorio"
#pile

XSatConstr: SExpressionOutputType with
    constr: (() -> Boolean, () -> SExpression) -> %
    run: % -> Boolean
== add
    Rep == Record(() -> Boolean, () -> SExpression)
    import from Rep

    constr(f: () -> Boolean, p: () -> SExpression): % == per [f, p]
    sexpression(c: %): SExpression ==
        (f, p) := explode rep c
	p()

    run(c: %): Boolean ==
        (f, p) := explode rep c
	f()

XSatResult: SExpressionOutputType with
    success: () -> %
    failed: () -> %

    test: % -> Boolean
    success: XSubstitution -> %

    success?: % -> Boolean
    failed?: % -> Boolean

    constraints: % -> List XSatConstr
    substs: % -> XSubstitution

    withConstraint: (%, XSatConstr) -> %
    export from XSatConstr
== add
    Rep == Record(state: MachineInteger, substs: XSubstitution, constr: List XSatConstr)
    import from Rep
    import from List XSatConstr
    import from MachineInteger, XSatConstr
    
    failed(): % == per [0, empty(), []]
    success(): % == per [1, empty(), []]
    anyToNone(): % == per [2, empty(), []]

    test(xs: %): Boolean == success? xs
    success(xs: XSubstitution): % == per [1, xs, []]

    success?(r: %): Boolean == rep(r).state = 1
    failed?(r: %): Boolean == rep(r).state = 2

    success(c: XSatConstr): % == per [1, empty(), [c]]

    result(b: Boolean): % == if b then success() else failed()
    constraints(r: %): List XSatConstr == rep(r).constr
    substs(r: %): XSubstitution == rep(r).substs

    withConstraint(r: %, c: XSatConstr): % == per [state(r), substs(r), cons(c, constraints r)]
    
    local state(r: %): MachineInteger == rep(r).state

    sexpression(r: %): SExpression ==
        import from Symbol
        empty? substs r => [if r then sexpr(-"success") else sexpr(-"failed")]
        [if success? r then sexpr(-"success") else sexpr(-"failed"), sexpression substs r]

XSubstitution: SExpressionOutputType with
    --PrimitiveType
    failed: () -> %
    failed?: % -> Boolean
    empty: FVSet -> %
    empty: () -> %
    copy: % -> %
    add!: (%, SymbolMeaning, Expression) -> ()
    alias!: (%, SymbolMeaning, SymbolMeaning) -> ()
    data: (%, SymbolMeaning) -> Partial Expression
    empty?: % -> Boolean
    expression: (%, Expression) -> Expression
    merge: (%, %) -> %
== add
    Rep == Record(fail: Boolean, fv: FVSet, uf: UFind(SymbolMeaning, Expression))
    import from List Expression
    import from MachineInteger
    import from Partial Expression
    import from Rep
    import from SymbolMeaning
    import from SymbolMeaningFields
    import from TermFields
    import from SExpression
    import from Expression
    import from XTypes

    failed?(xs: %): Boolean == rep(xs).fail
    empty(): % == empty empty()
    failed(): % == per [true, empty(), new()]
    empty(fv: FVSet): % == per [false, fv, new()]
    local urep(xs: %): UFind(SymbolMeaning, Expression) == rep(xs).uf
    local fvset(xs: %): FVSet == rep(xs).fv

    empty?(xs: %): Boolean == empty? urep xs

    copy(xs: %): % == per [failed? xs, fvset xs, copy urep xs]

    data(xs: %, sym: SymbolMeaning): Partial Expression == data(rep(xs).uf, sym)

    local setData!(xs: %, sym: SymbolMeaning, e: Expression): () ==
        isVar? e =>
	    stdout << "Setting var " << e << newline
	    never
	setData!(rep(xs).uf, sym, e)

    add!(xs: %, sym: SymbolMeaning, e: Expression): () ==
        --data(rep(xs).uf, sym) => never
	--if isVar? e then
	--    (v1, v2, xe) := union!(urep(xs), sym, syme termProperties e)
	--    stdout << "Skipping " << v1 << " " << v2 << " " << xe << newline
	--else
        setData!(xs, sym, e)

    alias!(xs: %, sym: SymbolMeaning, sym2: SymbolMeaning): () ==
        not isVar? sym or not isVar? sym2 => never
	stdout << "Alias: " << xs << " " << sym << " " << sym2 << newline
        union!(rep(xs).uf, sym, sym2)

    mergeCounter: CallCounter := counter("merge", false)
    merge(xs1: %, xs2: %): % ==
    	cc := open mergeCounter
        writer(cc) << cc << " Merge1 " << xs1 << newline
        writer(cc) << cc << " Merge2 " << xs2 << newline
	r := merge1(xs1, xs2)
        writer(cc) << cc << " Merge-Done " << r << newline
	close cc
	return r

    local merge1(xs1: %, xs2: %): % ==
        import from XSatisfier
        empty? xs1 => xs2
	empty? xs2 => xs1
	xs: XSubstitution := empty()
	cc := current(mergeCounter)
	for (v, p) in parents urep xs1 repeat
	    union!(urep xs, v, p)
	for (v, d) in data urep xs1 repeat
	    setData!(urep xs, v, d)

	for (v, p) in parents urep xs2 repeat
	    (v1, v2, old) := union!(urep xs, v, p)
	    writer(mergeCounter) << cc << " Merge-union " << v1 << " " << v2 << " " << old << newline

        writer(mergeCounter) << cc << " WithVars " << xs << newline
	for (v, d) in data urep xs2 repeat
	    writer(mergeCounter) << cc << " Check " << v << " " << d << " " << data(urep xs, v) << newline
	    
	for (v, d) in data urep xs2 repeat
	    writer(mergeCounter) << cc << " M: " << v << " " << d << newline
	    o := data(urep xs, v)
	    writer(mergeCounter) << cc << " Data: " << o << newline
	    if o then
	        (u, xs) := unify!(xs, d, retract o)
	        setData!(xs, v, u)
	        writer(mergeCounter) << cc << " Unify1: " << xs << newline
	    else
	        setData!(xs, v, d)
	        writer(mergeCounter) << cc << " Unify2: " << xs << newline
	xs

    expression(xs: %, e: Expression): Expression ==
        local expression(e: Expression): Expression == expression(xs, e)
	local data(t: SymbolMeaning): Partial Expression == data(urep xs, t)
	empty? xs => e
        term? e and isSet?(syme, termProperties e) =>
	    dd: Partial Expression := data(syme termProperties e)
	    failed? dd => e
	    expression retract dd
	term? e => e
	comma? e =>
	    comma([expression ei for ei in commaParts e])
	apply? e =>
	    apply(expression applicationOp e, [expression ei for ei in applicationArgs e])
	never

    sexpression(xs: %): SExpression ==
        import from Symbol, Expression
	failed? xs => [sexpr(-"fail")]
        [sexpr(-"xsubst"),
	 [sexpr(-"fv"), sexpression fvset xs],
	 [[sexpr(-"eq"), sexpression l, sexpression r] for (l, r) in parents urep xs],
	  [[sexpression v, sexpression d] for (v, d) in data urep xs]]

XSatisfier: with
    satisfies?: (TypeTerm, TypeTerm) -> XSatResult
    unify: (Expression, Expression) -> (Expression, XSubstitution)
    unify!: (XSubstitution, Expression, Expression) -> (Expression, XSubstitution)
    export from XSatResult
== add
    import from XTypes
    import from List Expression
    import from MachineInteger
    import from Symbol
    import from SymbolMeaning
    import from XSatConstr
    import from SExpression
    
    local satCounter: CallCounter := counter("tfsat", true)

    satisfies?(s: TypeTerm, t: TypeTerm): XSatResult ==
        cc := open satCounter
	writer(cc) << "(xsat " << s << " " << t << newline
	r := satisfies1?(s, t)
	writer(cc) << "unknown " << unknown? s << " " << unknown? t << newline
	writer(cc) << "constant " << constant? s << " " << constant? t << newline
	writer(cc) << "exit " << exit? s << " " << exit? t << newline
	writer(cc) << " xsat " << s << " " << t << " --> " << r << ")" << newline
	return r

    local satisfies1?(s: TypeTerm, t: TypeTerm): XSatResult ==
        unknown? t => success()
        unknown? s => failed()
	exit? s => success()
	tuple? t => error "tuple not implemented"
	multi? t => error "multi not implemented"
	cross? t => error "cross not implemented"
	map? t => error "map not implemented"
	cross? s => error "left cross not implemented"
	tuple? s => error "left tuple not implemented"
	constant? s and constant? t => satisfiesConst(s, t)
	satGeneric(s, t)

    local satGeneric(s: TypeTerm, t: TypeTerm): XSatResult == never

    satisfiesConst(s: TypeTerm, t: TypeTerm): XSatResult ==
        import from XSubstitution
        (e, xs) := unify(expr s, expr t)
	stdout << "SatisfiesConst " << s << " " << xs << newline
	success xs

    local unifyCounter: CallCounter := counter("unify", true)
    unify(s: Expression, t: Expression): (Expression, XSubstitution) == 
        unify!(empty(), s, t)

    unify!(xs: XSubstitution, e1: Expression, e2: Expression): (Expression, XSubstitution) == 
        constraints: List XSatConstr := []
        finalConstraints: List XSatConstr := []
	addConstraint!(c: XSatConstr): () ==
	    free constraints;
	    constraints := cons(c, constraints)
	addSubst!(syme: SymbolMeaning, e: Expression): () ==
	    addConstraint!(constr((): Boolean +-> doAddSubst!(syme, e), (): SExpression +-> [sexpr(-"subst"), sexpression syme, sexpression e]))
	unifyPair!(e1: Expression, e2: Expression): () ==
	    addConstraint!(constr((): Boolean +-> unify(e1, e2), (): SExpression +-> [sexpr(-"unify"), sexpression e1, sexpression e2]))
	unifyComma(e1: Expression, e2: Expression): Boolean ==
	    # commaParts e1 ~= # commaParts e2 => false
	    for ce1 in commaParts e1 for ce2 in commaParts e2 repeat
	        unifyPair!(ce1, ce2)
	    true
	unifyApply(e1: Expression, e2: Expression): Boolean ==
	    # applicationArgs e1 ~= # applicationArgs e2 => false
	    unifyPair!(applicationOp e1, applicationOp e2)
	    for ce1 in applicationArgs e1 for ce2 in applicationArgs e2 repeat
	        unifyPair!(ce1, ce2)
	    true
	unifyTermExpr(termExpr: Expression, e2: Expression): Boolean ==
	    not isVar? termExpr => termExpr = e2
	    addSubst!(syme termExpr, e2)
	    true
	unifySymbols(e1: Expression, e2: Expression): Boolean ==
	    not isVar? e1 and not isVar? e2 => e1 = e2
	    isVar? e1 =>
	        isVar? e2 and syme e1 = syme e2 => true
	        addSubst!(syme e1, e2)
		true
	    isVar? e2 =>
	        addSubst!(syme e2, e1)
		true
	    e1 = e2
	doAddSubst!(v: SymbolMeaning, e: Expression): Boolean ==
	    lhs: Partial Expression := data(xs, v)
	    rhs: Partial Expression := if isVar? e then data(xs, syme e) else failed
	    if lhs and rhs then
	        flg: Boolean := unify(retract lhs, retract rhs)
		if not flg then return false
		alias!(xs, v, syme e)
	    if not lhs and rhs then
	        stdout << "case 12" << newline
		alias!(xs, v, syme e)
	        add!(xs, v, retract rhs)
	    if lhs and not rhs then
	        if isVar? e then
		    alias!(xs, v, syme e)
		    add!(xs, v, retract lhs)
		else
		    flg: Boolean := unify(retract lhs, e)
		    if not flg then return false
		    add!(xs, v, e)
	    if not lhs and not rhs then
	        if isVar? e then
		    alias!(xs, v, syme e)
		else
		    add!(xs, v, e)
	    true
        unify(e1: Expression, e2: Expression): Boolean ==
            term? e1 and term? e2 =>
	        unifySymbols(e1, e2)
            term? e1 and not term? e2 =>
	        unifyTermExpr(e1, e2)
            term? e2 and not term? e1 =>
	        unifyTermExpr(e2, e1)
	    comma? e1 and not comma? e2 => false
	    comma? e1 and comma? e2 =>
	        unifyComma(e1, e2)
	    apply? e1 and not apply? e2 => false
	    apply? e1 and apply? e2 =>
	        unifyApply(e1, e2)
	    false
	cc := open(unifyCounter)
        writer(cc) << cc << " Unify Starts.. " << e1 << " " << e2 << newline
	unifyPair!(e1, e2)
        while not empty? constraints repeat
	    c := first constraints
	    writer(cc) << cc << " Unify - remaining: " << # constraints << newline
	    writer(cc) << cc << " Constr " << sexpression c << newline
	    constraints := rest constraints
	    r := run(c)
	    if not r then
	        writer(cc) << cc << " Unify failed.  " << c << newline
		close(cc)
	        return (e1, failed())
	    writer(cc) << cc <<  "Unify -> " << " " << xs << newline
	writer(cc) << cc << " Unify succeeded " << xs << newline
	close(cc)
	(e1, xs)

    import from SymbolMeaningFields, TermFields

XSatTestFunctions: with
    test: () -> ()
== add
    import from XSatisfier, TypeTerm, XSatResult
    
    test(): () ==
        testExit()
        testMerge1()
        testMerge2()
	testMerge3()
	
    local testExit(): () ==
        e1: TypeTerm := fromString "exit"
        e2: TypeTerm := fromString "bool"
	res := satisfies?(e1, e2)
        assert success? res

    -- (e1=e2) + (e1 -> bool)
    local testMerge1(): () ==
        import from XTypes, XSubstitution, Assert Expression
	e1 := newVar()
	e2 := newVar()
	bool := boolType()
        xs1 := empty()
        xs2 := empty()
	alias!(xs1, syme e1, syme e2)
	add!(xs2, syme e1, expr bool)
	stdout << "XS1 " << xs1 << newline
	stdout << "XS2 " << xs2 << newline
	assertEquals(expr bool, expression(xs2, expr e1))
	xsm := merge(xs1, xs2)
	stdout << "XSM " << xsm << newline
	assertEquals(expr bool, expression(xsm, expr e1))
	assertEquals(expr bool, expression(xsm, expr e2))

    -- Merge1 (xsubst (fv ()) () ((%v5 (apply map (comma %v6) %v7)) (%v6 int)))
    -- Merge2 (xsubst (fv ()) () ((%v5 (apply map (comma %v8) %v9))))

    local testMerge2(): () ==
        import from XTypes, XSubstitution, Assert Expression
	(e5, e6, e7, e8, e9) := (newVar(), newVar(), newVar(), newVar(), newVar())
	bool := boolType()
        xs1 := empty()
        xs2 := empty()
	add!(xs1, syme e5, expr map(e6, e7))
	add!(xs1, syme e6, expr bool)
	add!(xs2, syme e5, expr map(e8, e9))
	add!(xs1, syme e9, expr bool)
	xsm := merge(xs1, xs2)
	assertEquals(expr map(bool, bool), expression(xsm, expr e5))
	assertEquals(expr bool, expression(xsm, expr e8))

    -- Lazy merge1 ((forall () %v12) (xsubst (fv ()) ((eq %v11 %v12)) ()))
    -- Lazy merge2 ((forall () %v11) (xsubst (fv ()) ((eq %v11 %v12)) ((%v12 bool))))
    local testMerge3(): () ==
        import from XTypes, XSubstitution, Assert Expression
	(e5, e6) := (newVar(), newVar())
	bool := boolType()
        xs1 := empty()
        xs2 := empty()
	alias!(xs1, syme e5, syme e6)
	alias!(xs2, syme e5, syme e6)
	add!(xs2, syme e6, expr bool)
	xsm := merge(xs1, xs2)
	assertEquals(expr bool, expression(xsm, expr e6))
	assertEquals(expr bool, expression(xsm, expr e5))

#if ALDORTEST
#include "types"
#include "aldorio"
#pile

import from XTestInfer
test()$XSatTestFunctions
stdout << "DONE!" << newline
#endif
