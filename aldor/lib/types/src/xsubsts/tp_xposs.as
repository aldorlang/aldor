#include "types"
#include "aldorio"
#pile

-- NB: Consider XSubst as (v1 -> X1 and v2 -> X2 and ...)
-- Logically, tpi1 or tpi2 or ...
XTPoss: Join(PrimitiveType, SExpressionOutputType) with
    empty?: % -> Boolean
    unique?: % -> Boolean
    unique: % -> XTPossItem
    empty: () -> %

    bracket: Generator XTPossItem -> %
    bracket: Generator Cross(TypeTerm, XSubstitution) -> %
    bracket: Tuple TypeTerm -> %
    bracket: Tuple XTPossItem -> %

    generator: % -> Generator XTPossItem
    asConstraint: % -> XConstraintSet
    constrain: (%, XConstraintSet) -> %
    #: % -> MachineInteger

    -- return 
    merge: (%, %) -> %
    intersect: (%, %) -> %
    union: (%, %) -> %

    -- for each elt [i] in %, for each constraint [c], add constrain(i, c)

    export from XTPossItem

== add
    -- NB: Treat Rep as read-only
    Rep == List XTPossItem
    import from Rep
    import from XTPossItem
    import from MachineInteger
    import from BooleanFold
    import from TypeTerm
    import from XSatResult
    import from XSubstitution
    import from XSatisfier
    
    empty?(tp: %): Boolean == empty? rep tp
    unique?(tp: %): Boolean ==
        not empty? tp and empty? rest rep tp

    unique(tp: %): XTPossItem ==
        not unique? tp => never
	first rep tp

    empty(): % == per []
    #(tp: %): MachineInteger == # rep tp
    
    -- NB: Needs to take care of subst-equality
    (=)(a: %, b: %): Boolean ==
        not(#a = #b) => false
	for ai in a repeat
	    not(_or/(ai = bi for bi in b)) => return false
	return true
	
    bracket(g: Generator XTPossItem): % == per [tp for tp in g]
    bracket(g: Generator Cross(TypeTerm, XSubstitution)): % == per [item(tp, xs) for (tp, xs) in g]
    bracket(tt: Tuple TypeTerm): % == per [item element(tt, n) for n in 1..length tt]
    bracket(tt: Tuple XTPossItem): % == per [element(tt, n) for n in 1..length tt]

    generator(tp: %): Generator XTPossItem == ti for ti in rep tp

    sexpression(tp: %): SExpression == [sexpression part for part in rep tp]

    union(tp1: %, tp2: %): % == 
        gg := generate
            for tpi in tp1 repeat yield tpi
            for tpi in tp2 repeat yield tpi
        [gg]

    local intersectCounter: CallCounter := counter("tpossIntersect", true)
    intersect(tp1: %, tp2: %): % ==
        cc := open intersectCounter
        inner(): % ==
	    res: List XTPossItem := []
	    for tpi1 in tp1 repeat
	        tp2l: List XTPossItem := [tpi for tpi in tp2]
		found := false
		while not found repeat
		    candidate := first tp2l
		    tpl2 := rest tp2l
		    merged: XTPossItem := merge(tpi1, candidate)
		    if not failed? merged then
		        found := true
			res := cons(merged, res)
	        lineWriter(cc) << "Completed " << tpi1 << " " << found << newline
	    [item for item in res]
	lineWriter(cc) << "(start: " << tp1 << newline
	lineWriter(cc) << " other " << tp2 << newline
	val: % := inner()
	lineWriter(cc) << val << ")" << newline
	close cc
	return val

    local mergeCounter: CallCounter := counter("tpossMerge", true)
    merge(tp1: %, tp2: %): % ==
        cc := open mergeCounter
        writer(cc) << "XPoss Merge " << tp1 << " " << tp2 << newline
	all: List XTPossItem := []
	for tpi1 in tp1 repeat
	    -- filter to unique constraints
	    for tpi2 in tp2 repeat
	        merged: XTPossItem := merge(tpi1, tpi2)
		if not failed? merged then
		    writer(cc) << "Add " << tpi1 << " <> " << tpi2 << " --> " << merged << " " << newline
		    all := cons(merged, all)
        writer(cc) << "XPoss Merge done " << all << newline
	close(cc)
	[tpi for tpi in all]

    constrain(tposs: %, cs: XConstraintSet): % ==
        ll: List XTPossItem := []
        for tpi in tposs repeat
	    for c in cs repeat
	        tpi2: XTPossItem := constrain(tpi, c)
		if not failed? tpi2 then
		    ll := cons(tpi2, ll)
        [tpi for tpi in ll]

    asConstraint(tposs: %): XConstraintSet ==
        ll: List XSubstitution := []
	for tpi in tposs repeat
	    if empty? constraint tpi then return empty()
	for tpi in tposs repeat
	    if not failed? tpi then ll := cons(constraint tpi, ll)
	[generator ll]

-- Logically, C1 or C2 or...
XConstraintSet: Join(PrimitiveType, SExpressionOutputType) with
    empty: () -> %
    one: XSubstitution -> %
    bracket: Generator XSubstitution -> %
    generator: % -> Generator XSubstitution
    merge: (%, %) -> %
    failed?: % -> Boolean
== add
    Rep == List XSubstitution
    import from Rep
    import from XSubstitution

    empty(): % == one empty()
    failed?(cs: %): Boolean == empty? rep cs
    one(xs: XSubstitution): % == per [xs]
    bracket(g: Generator XSubstitution): % == per bracket g
    generator(cs: %): Generator XSubstitution == generator rep cs

    merge(cs1: %, cs2: %): % ==
	failed? cs1 => cs1
	failed? cs2 => cs2
        stdout << "CS Merge " << cs1 << " " << cs2 << newline
	ll : List XSubstitution := []
	for s1 in cs1 repeat
	    for s2 in cs2 repeat
	        merged := merge(s1, s2)
		if not failed? merged then
		    ll := cons(merged, ll)
        stdout << "CS Merge - Result " << ll << newline
	[s for s in ll]
	
    sexpression(cs: %): SExpression ==
        import from Symbol
        cons(sexpr(-"cs"), [sexpression c for c in cs])

    (=)(cs1: %, cs2: %): Boolean == never

-- Logically, (T=? and Substs=?)
XTPossItem: Join(PrimitiveType, SExpressionOutputType) with
    item: TypeTerm -> %
    item: (TypeTerm, XSubstitution) -> %
    failed: () -> %
    typeTerm:   % -> TypeTerm
    constraint: % -> XSubstitution
    constrain: (%, XSubstitution) -> %

--    satisfies?: (%, %) -> XSatResult
    failed?: % -> Boolean
    merge: (%, %) -> %
    expand: % -> Expression
    expand: % -> TypeTerm

    project: (TypeTerm -> TypeTerm) -> (% -> %)
    project: (TypeTerm -> List TypeTerm) -> (% -> List %)
== add
    import from XSatisfier
    import from Expression
    
    Rep == Record(tt: TypeTerm, xs: XSubstitution)
    import from Rep, XSubstitution
    
    item(tt: TypeTerm): % == per [tt, empty()]
    item(tt: TypeTerm, xs: XSubstitution): % == per [tt, xs]
    item(xs: XSubstitution): % == per [unknown(), xs]

    typeTerm(ti: %): TypeTerm == rep(ti).tt
    constraint(ti: %): XSubstitution == rep(ti).xs

    failed?(ti: %): Boolean == failed? constraint ti
    failed(): % == per [exitType()$XTypes, failed()]

    expand(tpi: %): Expression ==
        stdout << "Expand " << tpi << " --> " << expression(constraint tpi, expr typeTerm tpi) << newline
        expression(constraint tpi, expr typeTerm tpi)

    expand(tpi: %): TypeTerm ==
        constant expression(constraint tpi, expr typeTerm tpi)

    satisfies?(ti1: %, ti2: %): XSatResult ==
        -- I'd prefer this to be lazy, but...
        satisfies?(constant expand ti1, constant expand ti2)

    project(f: TypeTerm -> TypeTerm)(p: %): % ==
        item(f typeTerm p, constraint p)

    project(f: TypeTerm -> List TypeTerm)(p: %): List % ==
        import from List TypeTerm
        [item(x, constraint p) for x in f typeTerm p]

    constrain(ti: %, xs: XSubstitution): % ==
        empty? xs => ti
	failed? xs => failed()
        item(typeTerm ti, merge(constraint ti, xs))

    local mergeCounter: CallCounter := counter("tpossItemMerge", false)
    merge(ti1: %, ti2: %): % ==
        cc := open(mergeCounter)
        writer(cc) << cc << " TPI Merge " << ti1 << " "  << ti2 << newline
        failed? ti1 or failed? ti2 => item(typeTerm ti1, failed())
        ps := merge(constraint ti1, constraint ti2)
        writer(cc) << cc << " TPI Merge - ps " << ps << newline
	(e, xs) := unify!(ps, expr typeTerm ti1, expr typeTerm ti2)
        writer(cc) << cc << " TPI Merge - u " << e << " " << xs << newline
	close(mergeCounter)
	item(constant e, xs)
	
    (=)(a: %, b: %): Boolean ==
        import from Expression
        expression(constraint a, expr typeTerm a) = expression(constraint b, expr typeTerm b)

    sexpression(ti: %): SExpression ==
        if empty? constraint ti then sexpression(typeTerm ti) else [sexpression typeTerm ti, sexpression constraint ti]

TestXPoss: with
    test: () -> ()
== add
    import from TypeTerm
   
    test(): () ==
        testMerge1()
        testMerge2()

    -- [(bool, ())], [(int, ()), (bool)]
    local testMerge1(): () ==
        import from XTypes, XSubstitution, Assert Expression
	bool := boolType()
        xp1: XTPoss := [boolType()]
        xp2: XTPoss := [boolType(), intType()]
	xpm := merge(xp1, xp2)
	stdout << "MM1: " << xpm << newline
	assertTrue(unique? xpm)

    -- [(bool, ())], [(int, ()), (bool)]
    local testMerge2(): () ==
        import from XTypes, XSubstitution, Assert Expression, Assert TypeTerm
	(e1, e2) := (newVar(), newVar())
	bool := boolType()
        xp1: XTPoss := [boolType()]
        xp2: XTPoss := [e1]
	xpm := merge(xp1, xp2)
	stdout << "MM: " << xpm << newline
	assertTrue(unique? xpm)
	assertEquals(boolType(), typeTerm unique xpm)




#if ALDORTEST
#include "types"
#include "aldorio"
#pile

test()$TestXPoss

#endif
