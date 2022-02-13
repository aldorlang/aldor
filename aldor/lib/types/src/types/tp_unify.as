#include "types"
#include "aldorio"
#pile

UnificationResult: SExpressionOutputType with
    sigma: % -> Substitution
    fv: % -> ListSet Symbol

    emptyResult: () -> %
    result: (Substitution, ListSet Symbol, List TypeTerm) -> %
    result: (Substitution, ListSet Symbol) -> %
    failed: () -> %
    failed?: % -> Boolean

    validate: % -> List String
== add
    Rep == Record(failed: Boolean, subst: Substitution, fv: ListSet Symbol, params: List TypeTerm)
    import from Rep
    import from List ExpressionTree, ExpressionTreeLeaf
    import from String, Symbol
    import from SExpressionBuilder
    import from ListSExpression TypeTerm
    import from ListSet Symbol

    theFailed: % == per [true, empty(), [], []]

    result(sigma: Substitution, fv: ListSet Symbol, pl: List TypeTerm): % == per [false, sigma, fv, pl]
    result(sigma: Substitution, fv: ListSet Symbol): % == per [false, sigma, fv, []]

    failed(): % == theFailed;
    failed?(r: %): Boolean == rep(r).failed

    emptyResult(): % == per [false, empty(), [], []]
    sigma(r: %): Substitution == rep(r).subst
    fv(r: %): ListSet Symbol == rep(r).fv
    params(r: %): List TypeTerm == rep(r).params

    (a: %) = (b: %): Boolean == failed? a =
        failed? b
          and sigma a = sigma b
	  and fv a = fv b
	  and params a = params b
	  

    validate(r: %): List String ==
        import from MachineInteger, Substitution, Expression, List Cross(Symbol, Expression)
        issues: List String := []
        sigmaR := sigma r
        substVars: ListSet Symbol := [v for (v, e) in substs sigmaR]
        if # substVars ~= # substs sigmaR then issues := cons("Repeated substs ", issues)
        for (var, expr) in substs sigmaR repeat
            if terms expr - substVars ~= terms expr then issues := cons("Substs on rhs ", issues)
        issues

    sexpression(ur: %): SExpression ==
        sb: SExpressionBuilder := sxbuilder()
	sb << (-"result")
	if failed? ur then sb << "failed"
	else
	    if not empty? sigma ur then sb << [sexpr(-"sigma"), sexpression sigma ur]
	    if not empty? fv ur then sb << [sexpr(-"fv"), [sexpr v for v in fv ur]]
	    if not empty? params ur then sb << [sexpr(-"params"), listSExpression params ur]
        build sb

UnificationTools: with
    unify: (TypeTerm, TypeTerm) -> UnificationResult
    apply: (UnificationResult, TypeTerm) -> TypeTerm
    apply: (UnificationResult, ParamSet) -> ParamSet
    mergeResults: (UnificationResult, UnificationResult) -> UnificationResult

    unifyParameters: (ParamSet, ParamSet) -> (UnificationResult, ParamSet)

    export from UnificationResult
== add
    import from Expression, Substitution
    import from ListSet Symbol, Symbol
    import from TypeTerm
    import from MachineInteger
    import from List Expression
    import from Expression
    
    unifyCounter: CallCounter := counter("unify", false) 
    unify(tf1: TypeTerm, tf2: TypeTerm): UnificationResult ==
        count := open(unifyCounter)
        writer(count) << "(Unify " << count << ": " << tf1 << " <--> " << tf2 << newline
        result := unify1(tf1, tf2)
        writer(count) << " Unify " << count << ": " << tf1 << " <--> " << tf2 << " ===> " << result << ")" << newline
        validate(result)
	close(unifyCounter)
        result

    local unify1(tf1: TypeTerm, tf2: TypeTerm): UnificationResult ==
        term? expr tf2 and not term? expr tf1 => unify(tf2, tf1)
        term? expr tf1 and term? expr tf2 =>
            var?(tf1, term expr tf1) and var?(tf2, term expr tf2) =>
                newVar: Symbol := new()
		writer(unifyCounter) << " both vars - " << expr tf1 << " " << expr tf2 << " --> " << newVar << newline
		subst := sigma( (term expr tf1, expr newVar), (term expr tf2, expr newVar))
		writer(unifyCounter) << " sigma " << subst << newline
                result(subst, [newVar])
            var?(tf1, term expr tf1) =>
                result(sigma(term expr tf1, expr tf2), [])
            var?(tf2, term expr tf2) =>
                result(sigma(term expr tf2, expr tf1), [])
            if term expr tf1 = term expr tf2 then result( empty(), vars tf1 + vars tf2) else failed()
        term? expr tf1 =>
            var?(tf1, term expr tf1) => result(sigma(term expr tf1, expr tf2), vars tf2)
            failed()
        application? expr tf1 and not application? expr tf2 =>
            failed()
        application? expr tf1 =>
	    r1 := unify(forall(vars tf1, applicationOp expr tf1), forall(vars tf2, applicationOp expr tf2))
	    # applicationArgs expr tf1 ~= # applicationArgs expr tf2 => failed()
	    for tf1i in applicationArgs expr tf1 for tf2i in applicationArgs expr tf2 repeat
                r2 := unify(forall(vars tf1, tf1i), forall(vars tf2, tf2i))
                r1 := mergeResults(r1, r2)
	    return r1
	comma? expr tf1 and not comma? expr tf2 => failed()
	comma? expr tf1 and comma? expr tf2 =>
	    # commaParts expr tf1 ~= # commaParts expr tf2 => failed()
	    r1 := emptyResult()
	    for tfi1 in commaParts expr tf1 for tfi2 in commaParts expr tf2 repeat
	        r := unify(forall(vars tf1, tfi1), forall(vars tf2, tfi2))
		r1 := mergeResults(r1, r)
	    return r1
	param? expr tf1 and not param? expr tf2 => failed()
	param? expr tf1 and param? expr tf2 =>
	    paramVar expr tf1 = paramVar expr tf2 =>
	        return unify(forall(vars tf1, paramExpr expr tf1), forall(vars tf2, paramExpr expr tf2))
	    failed()
        stdout << "Missing case " << tf1 << " + " << tf2 << newline
        never

    mergeCounter: CallCounter := counter("merge", false) 
    mergeResults(r1: UnificationResult, r2: UnificationResult): UnificationResult ==
        count := open(mergeCounter)
        writer(count) << "(Merge " << count << ": " << r1 << " + " << r2 << newline
        res := mergeResults1(r1, r2)
        writer(count) << " Merge " << count << ": " << r1 << " + " << r2 << " ==> " << res << ")" << newline
        valid: List String := validate(res)
        if not empty? valid then
            stdout << "Invalid Result " << valid << newline
            never
	close(count)
        res

    local mergeResults1(r1: UnificationResult, r2: UnificationResult): UnificationResult ==
        failed? r1 => r1
	failed? r2 => r2
        final: UnificationResult := result(sigma r1, fv r1 + fv r2)
        sigma2 := sigma r2
        while not empty? sigma2 repeat
            (var, expr, rest) := split(sigma2)
            sigma2 := rest
            expr := (sigma final) expr
            maybeOther: Partial Expression := lookup(sigma final, var)
            if failed? maybeOther then
                sig0: Substitution := sigma(var, expr)
                final := result(addSubst(sigma( (var1, sig0 expr1) for (var1, expr1) in sigma final), var, expr), fv final - [var])
            else
                -- replace final with substituted and add (var, expr) as a new substitution
                other := retract maybeOther
                innerUnify := unify(forall(fv final, other), forall(fv final, expr))
                newSigma := addSubst(sigma( (v, e) for (v, e) in sigma final | v ~= var), var, (sigma innerUnify) expr)
                final := mergeResults(result(newSigma, fv final), innerUnify)
            writer(unifyCounter) << " Merge Step " << var << " -> " << expr << " ==> " << final << newline
        validate final
        final

    unifyParameters(tt1: ParamSet, tt2: ParamSet): (UnificationResult, ParamSet) ==
        import from Partial TypeTerm
        local lhsSplit: List Cross(TypeTerm, Partial TypeTerm)
	lhsSplit := [(p, find(paramVar expr p, tt2)) for p in tt1]
	lhs: ParamSet := [p1 for (p1, pp2) in lhsSplit | not pp2]
        rhs: ParamSet := [p2 for p2 in tt2 | not find(paramVar expr p2, tt1)]
	ur := result(empty(), [])
	mid: List TypeTerm := []
	for (p1, pp2) in lhsSplit repeat
	    failed? pp2 => iterate
	    up := unify(p1, retract pp2)
	    ur := mergeResults(ur, up)
	    failed? ur => return (failed(), empty())
	    pfinal := apply(ur, p1)
	    mid := cons(pfinal, mid)
	return (ur, ur lhs + ur rhs + [tt for tt in mid])

    apply(result: UnificationResult, tt: TypeTerm): TypeTerm ==
        import from ListSet Symbol, List Cross(Symbol, Expression)
        newExpr := (sigma result) expr tt
	ls: ListSet Symbol := []
	substVars: ListSet Symbol := [sym for (sym, expr) in sigma result]
        forall(fv result + vars tt - substVars, newExpr)

    apply(result: UnificationResult, ps: ParamSet): ParamSet ==
        import from ListSet Symbol, List Cross(Symbol, Expression)
	[apply(result, param) for param in ps]

TestUnify: with
    testUnify: () -> ()
== add
    import from UnificationTools
    import from Substitution
    import from TypeTerm
    import from Assert TypeTerm
    import from Assert ListSet Symbol
    import from Assert Substitution
    import from ListSet Symbol
    import from Symbol
    import from Expression
    
    testUnify(): () ==
        testUnifySimple1()
        testUnifySimple2()
	testUnifyApply1()
	testUnifyApply2()
	testUnifyParamSet()
	testUnifyParamSet2()
	testUnifyParamSet3()
	testUnifyMerge()
	
    local test(tt1: TypeTerm, tt2: TypeTerm): () ==
        r: UnificationResult := unify(tt1, tt2)
        assertEquals(r tt1, r tt2)

    testUnifySimple1(): () ==
        import from UnificationResult, Expression
        e1: TypeTerm := fromString "(forall (x) x)"
	e2: TypeTerm := fromString "(apply foo a)"
	r := unify(e1, e2)
	expectedSigma: Substitution := sigma(-"x", fromString "(apply foo a)")
	assertEquals(expectedSigma, sigma r)
	assertTrue(empty? fv r)

    testUnifySimple2(): () ==
        import from UnificationResult, Expression
        e1: TypeTerm := fromString "(forall (x) x)"
	e2: TypeTerm := fromString "(forall (k) k)"
	test(e1, e2)

    testUnifyApply1(): () ==
        e1: TypeTerm := fromString "(forall (x) (apply foo x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(e1, e2)

    testUnifyApply2(): () ==
        e1: TypeTerm := fromString "(forall (f) (apply f x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(e1, e2)

    testUnifyMerge(): () ==
        r1: UnificationResult := result(empty(), [-"a"])
	r2: UnificationResult := result(addSubst(empty(), -"a", fromString "int"), [])
	m := mergeResults(r1, r2)
	assertEquals([], fv m)

    testUnifyParamSet(): () ==
        import from Partial TypeTerm
        local ps1, ps2: ParamSet    			 
        ps1 := [fromString "(forall (a) (param x a))"]
	ps2 := [fromString "(param y int)"]
	(urF, psrF) := unifyParameters(ps1, ps2)
	(urB, psrB) := unifyParameters(ps2, ps1)
	assertEquals([-"a"], fv ps1)
	assertEquals([], fv ps2)
	assertEquals([-"a"], fv psrF)
	assertEquals([-"a"], fv psrB)
	assertTrue(empty? sigma urF)
	assertTrue(empty? sigma urB)
	assertEquals(fromString "(forall (a) (param x a))", retract find(-"x", ps1))
	assertEquals(fromString "(forall (a) (param x a))", retract find(-"x", psrF))
	assertEquals(fromString "(forall (a) (param x a))", retract find(-"x", psrB))

    testUnifyParamSet2(): () ==
        import from Partial TypeTerm
        local ps1, ps2: ParamSet    			 
        ps1 := [fromString "(forall (a) (param x a))"]
	ps2 := [fromString "(forall (b) (param x (apply list b)))"]
	(urF, psrF) := unifyParameters(ps1, ps2)
	(urB, psrB) := unifyParameters(ps2, ps1)
	assertEquals(fromString "(forall (c) (param x (apply list c)))", retract find(-"x", psrF))
	assertEquals(fromString "(forall (c) (param x (apply list c)))", retract find(-"x", psrB))

    testUnifyParamSet3(): () ==
        import from Partial TypeTerm
        local ps1, ps2: ParamSet    			 
        ps1 := [fromString "(forall (a) (param x a))", fromString "(forall (a) (param y a))"]
	ps2 := [fromString "(forall (b) (param x (apply list b)))", fromString "(forall (b) (param z b))"]
	(urF, psrF) := unifyParameters(ps1, ps2)
	(urB, psrB) := unifyParameters(ps2, ps1)
	stdout << "psrF: " << psrF << newline
	stdout << "psrB: " << psrB << newline
	assertEquals(fromString "(forall (c) (param x (apply list c)) (param y (list c)) (param z c)))", retract find(-"x", psrF))
	assertEquals(fromString "(forall (c) (param x (apply list c)) (param y (list c)) (param z c)))", retract find(-"x", psrB))
	assertEquals([-"b"], fv psrF)
	assertEquals([-"b"], fv psrB)

#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestUnify
testUnify()
stdout << "DONE!" << newline
#endif
