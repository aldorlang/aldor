#include "types"
#include "aldorio"
#pile

UnificationResult: SExpressionOutputType with
    sigma: % -> Substitution
    fv: % -> ListSet Symbol

    result: (Substitution, ListSet Symbol) -> %
    failed: () -> %
    failed?: % -> Boolean

    validate: % -> List String
== add
    Rep == Record(failed: Boolean, subst: Substitution, fv: ListSet Symbol)
    import from Rep
    import from List ExpressionTree, ExpressionTreeLeaf
    import from String, Symbol
    import from SExpressionBuilder
    
    theFailed: % == per [true, empty(), []]

    result(sigma: Substitution, fv: ListSet Symbol): % == per [false, sigma, fv]

    failed(): % == theFailed;
    failed?(r: %): Boolean == rep(r).failed

    sigma(r: %): Substitution == rep(r).subst
    fv(r: %): ListSet Symbol == rep(r).fv

    (a: %) = (b: %): Boolean == failed? a = failed? b and sigma a = sigma b and fv a = fv b

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
	    sb << [cons(sexpr(-"sigma"), sexpression sigma ur),
	           cons(sexpr(-"fv"), [sexpr v for v in fv ur])]
        build sb
	
Operators: with
    unify: (TypeTerm, TypeTerm) -> UnificationResult
    apply: (UnificationResult, TypeTerm) -> TypeTerm
    mergeResults: (UnificationResult, UnificationResult) -> UnificationResult
== add
    import from Expression, Substitution
    import from ListSet Symbol, Symbol
    import from TypeTerm
    import from MachineInteger
    import from List Expression

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
        stdout << "Missing case (apply) " << tf1 << " + " << tf2 << newline
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

    apply(result: UnificationResult, tt: TypeTerm): TypeTerm ==
        import from ListSet Symbol, List Cross(Symbol, Expression)
        newExpr := (sigma result) expr tt
	ls: ListSet Symbol := []
	substVars: ListSet Symbol := [sym for (sym, expr) in sigma result]
        forall(fv result + vars tt - substVars, newExpr)


ScopeBind: with
    bind: (Unit Expression, e: Env) -> ()
    phase: NamedAnnotation Env
    annotations: Unit Expression -> Annotated(Expression, Env)
== add
    import from List Expression
    import from Expression
    import from TypeTerm
    import from Symbol

    phase: NamedAnnotation Env == new "bind"
    annotations(u: Unit Expression): Annotated(Expression, Env) == annotations(u, Env, phase)

    bind(u: Unit Expression, env: Env): () ==
        tbl: Annotated(Expression, Env) := new()
	bindInner(ee: Env, e: Expression): () ==
	    lambda? e =>
	        newEnv := push(ee)
		populateLambdaLevel(newEnv, e)
	        annotate!(tbl, e, newEnv)
		bindInner(newEnv, lambdaBody(e))
	    for p in parts e repeat
	        bindInner(ee, p)
        annotate!(tbl, base u, env)
        bindInner(env, base u)
	register(u, Env, phase, tbl)

    local populateLambdaLevel(env: Env, expr: Expression): () ==
        for var in lambdaVars expr repeat
	    declare? var =>
	        put!(env, declareVar var, constant declareType var)
	    term? var => put!(env, term var, constant(expr(-"unknown")$Expression))
	    error "Unknown var"
	    
BottomUp: with
    bupAnnotations: Unit Expression -> Annotated(Expression, List TypeTerm)
    infer: (Env, Expression) -> Annotated(Expression, List TypeTerm)
    infer: Unit Expression -> Annotated(Expression, List TypeTerm)
    phase: NamedAnnotation List TypeTerm
== add
    import from List Expression, List TypeTerm
    import from List List TypeTerm
    import from ListSet Symbol
    import from Expression
    import from Substitution
    import from Symbol
    import from Operators
    import from MachineInteger
    import from ListSet Symbol
    import from TypeTerm
    import from UnificationResult

    phase: NamedAnnotation List TypeTerm == new "bup"

    bupAnnotations(u: Unit Expression): Annotated(Expression, List TypeTerm) ==
        annotations(u, List TypeTerm, phase)

    infer(unit: Unit Expression): Annotated(Expression, List TypeTerm) ==
        envAnnotations: Annotated(Expression, Env) := annotations(unit)$ScopeBind
        rootEnv: Env := annotation(envAnnotations, base unit)
        bup := infer(rootEnv, base unit)
	register(unit, List TypeTerm, phase, bup)
	bup
	
    infer(e: Env, expr: Expression): Annotated(Expression, List TypeTerm) ==
        tbl: Annotated(Expression, List TypeTerm) := new()
        inferExpression(e: Expression): () ==
	    stdout << "(infer " << e << newline
	    inferExpression1 e
	    stdout << " infer " << e << " --> " << annotation(tbl, e) << ")" << newline
        inferExpression1(e: Expression): () ==
	    term? e => inferTerm e
	    apply? e => inferApply e
	    if? e => inferIf e
	    lambda? e => inferLambda e
	    error "Missing infer case"
	instantiate(t: TypeTerm): TypeTerm ==
	    constant? t => t
	    newVars: ListSet Symbol := [new()$Symbol for oldvar in vars t]
	    newSubst := sigma((oldvar, expr(newvar)$Expression) for oldvar in vars t for newvar in newVars)
	    stdout << "Instantiate " << t << newline
	    stdout << "Instantiate - subst " << newSubst << newline
	    forall(newVars, apply(newSubst, expr(t)$TypeTerm))
        inferTerm(termExpr: Expression): () ==
	    annotate!(tbl, termExpr, [instantiate t for t in meanings(e, term termExpr)])
	inferApply(app: Expression): () ==
	    inferExpression(applicationOp app)
	    for arg in applicationArgs app repeat
	        inferExpression(arg)
            tOp: List TypeTerm := annotation(tbl, applicationOp app)
            maps := filterMaps(tOp)
	    annotate!(tbl, app, filterOps(tOp, [annotation(tbl, arg) for arg in applicationArgs app]))
	inferLambda(e: Expression): () ==
	    types := inferLambda1(inferExpression, tbl, e)
	    annotate!(tbl, e, types)
	inferIf(app: Expression): () ==
	    inferExpression(ifTest app)
	    inferExpression(ifPart app)
	    inferExpression(ifElsePart app)
	    poss: List UnificationResult := []
	    for tt in annotation(tbl, ifTest app) repeat
	        ur := unify(tt, fromString "bool")
		if not failed? ur then
		   poss := cons(ur, poss)
            tposs: List TypeTerm := []
	    for pp in poss repeat
	        for ttIf in annotation(tbl, ifPart app) repeat
	           for ttElse in annotation(tbl, ifElsePart app) repeat
		       ttIfx := pp ttIf
		       ttElsex := pp ttElse
		       stdout << "if unify " << ttIfx << " <> " << ttElsex << newline
		       ur := unify(ttIfx, ttElsex)
		       ur := mergeResults(pp, ur)
		       if not failed? ur then
		           stdout << "found " << ttIf << newline
		           tposs := cons(ur ttIf, tposs)
	    stdout << "if tposs " << tposs << newline
	    annotate!(tbl, app, tposs)
        inferExpression expr
	return tbl

    inferLambda1(infer: Expression -> (), tbl: Annotated(Expression, List TypeTerm), e: Expression): List TypeTerm ==
        makeLambda(retType: TypeTerm): TypeTerm ==
	    mapExpr: Expression := apply(expr(-"map"), [comma([declareType decl for decl in lambdaVars e]), expr retType])
            forall(vars retType, mapExpr)
        infer(lambdaBody e)
	for var in lambdaVars e repeat
	    not declare? var => "error - all lambda vars must be declared"
	types := annotation(tbl, lambdaBody e)
	[makeLambda(type) for type in types]

    filterMaps(ops: List TypeTerm): List TypeTerm == [t for t in ops | map? t]
  
    filterOps(ops: List TypeTerm, argPoss: List List TypeTerm): List TypeTerm ==
        stdout << "Ops " << ops << newline
        stdout << "args " << argPoss << newline
	possR: List TypeTerm := []
	for opType in ops repeat
	    possl: List UnificationResult := [result(empty(), [])]
	    for argNPoss in argPoss for i in 1.. repeat
	        opArgTf := mapArgN(opType, i)
	        stdout << "(Considering argument " << i << " = " << opArgTf << newline
	        newPossl: List UnificationResult := []
	        for argTf in argNPoss for j in 1.. repeat
		    stdout << "Type " << i << "," << j << "/" << (#argNPoss) << " " << opArgTf << " <> " << argTf << newline
                    ur := unify(opArgTf, argTf)
		    stdout << "Type " << i << "," << j << "/" << (#argNPoss) << " = " << ur << newline
		    stdout << "PossL " << i << "," << j << "/" << (#argNPoss) << " = " << possl << newline
                    if not failed? ur then
		        for poss in possl repeat
			    urx := mergeResults(ur, poss)
		            if not failed? urx then
			        newPossl := cons(urx, newPossl)
			    stdout << "Poss " << j << " " << urx << newline
	        possl := newPossl
		stdout << "Finished arg " << i << " " << possl << ")" << newline
	    stdout << "OpType " << opType << newline
	    stdout << "OpType - Results " << possl << newline
	    possR := append!([possN mapRet opType for possN in possl], possR)
	stdout << "Types " << possR << newline
        return possR
	
    map?(t: TypeTerm): Boolean == applyOf?(-"Map", expr t)

    mapArgs(t: TypeTerm): List TypeTerm == [forall(vars t, argExpr) for argExpr in commaParts first applicationArgs expr t]
    mapArgN(t: TypeTerm, n: MachineInteger): TypeTerm == 
        forall(vars t, apply(commaParts(first applicationArgs expr t), n))

    mapRet(t: TypeTerm): TypeTerm == forall(vars t, first rest applicationArgs expr t)

    applyOf?(sym: Symbol, e: Expression): Boolean ==
        apply? e and term? applicationOp e and term applicationOp e = sym

TopDown: with
    tdnAnnotations: Unit Expression -> Annotated(Expression, TypeTerm)
    infer: Unit Expression -> Annotated(Expression, TypeTerm)
    phase: NamedAnnotation TypeTerm
== add
    import from BottomUp
    
    phase: NamedAnnotation TypeTerm == new "tdn"
    tdnAnnotations(u: Unit Expression): Annotated(Expression, TypeTerm) == annotations(u, TypeTerm, phase)
    
    infer(unit: Unit Expression): Annotated(Expression, TypeTerm) == never

    infer(unit: Unit Expression, topTf: TypeTerm): () == never


TestSubstitution: with
    testExpression: () -> ()
    testSubst: () -> ()
    testUnify: () -> ()
    testBottomUp: () -> ()
== add
    import from Symbol, String
    import from Operators
    import from ListSet Symbol
    import from Assert TypeTerm
    import from Assert List TypeTerm
    import from Assert Expression
    import from Assert SExpression
    import from Assert Substitution
    import from Env
    import from Unit Expression
    import from TypeTerm
    import from BottomUp
    import from ScopeBind
    import from Annotated(Expression, List TypeTerm)
    import from List TypeTerm
    import from Partial Expression
    
    local test(tt1: TypeTerm, tt2: TypeTerm): () ==
        r: UnificationResult := unify(tt1, tt2)
	stdout << "r " << r << newline
	stdout << "r 2 " << r tt1 << newline
	stdout << "r 1 " << r tt2 << newline
        assertEquals(r tt1, r tt2)

    testExpression(): () ==
        import from Expression
        sx: SExpression := fromString "(|apply| |f| |x|)"
        e: Expression := fromString "(apply f x)"
	assertEquals(sexpression e, sx)

    testSubst(): () ==
        sigma: Substitution := sigma(-"x", fromString "(apply f a)")
	expr: Expression := fromString "(apply foo x)"
	assertEquals(fromString "(apply foo (apply f a))", sigma expr)

    testUnify(): () ==
        testUnifySimple1()
        testUnifySimple2()
	testUnifyApply1()
	testUnifyApply2()

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

    testBottomUp(): () ==
        testBottomUpSimple()
	testBottomUpVar1()
	testBottomUpVar1Shared()
	testBottomUpVar2()
	testBottomUpVar3()
	testBottomUpIf()
	testBottomUpLambda()
	
    testBottomUpSimple(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
        put!(env, -"+", fromString "(apply map (comma int int) int)")
	expr: Expression := fromString "n"

	unit: Unit Expression := new expr
	bind(unit, env)
	bup := infer(unit)
	tl := annotation(bup, expr)
	assertEquals([fromString "int"], tl)

	expr2: Expression := fromString "(apply + n n)"
	bup := infer(env, expr2)
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr2))

    testBottomUpVar1(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	bup := infer(env, expr)
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpVar1Shared(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"nil", fromString "(forall (t) (apply list t))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons nil nil)"
	bup := infer(env, expr)
	assertEquals([fromString "(forall (x) (apply list (apply list x)))"], annotation(bup, expr))

    testBottomUpVar2(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"n", fromString "float")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply list float)", fromString "(apply list int)"], annotation(bup, expr))
	
    testBottomUpVar3(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"s", fromString "string")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
	put!(env, -"bracket", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	
	expr: Expression := fromString "(apply bracket (apply cons n nil) s)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply pair (apply list int) string)"], annotation(bup, expr))

    testBottomUpIf(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"s", fromString "string")
	put!(env, -"=", fromString "(forall (t) (apply map (comma t t) bool))")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
    
	expr: Expression := fromString "(if (apply = n n) s s)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([fromString "string"], annotation(bup, expr))
	assertEquals([fromString "bool"], annotation(bup, retract search(expr, fromString ("(apply = n n)"))))

	expr: Expression := fromString "(if (apply = n n) s n)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([], annotation(bup, expr))

    testBottomUpLambda(): () ==
	expr: Expression := fromString "(lambda ((: x int)) x)"
	unit: Unit Expression := new(expr)
	bind(unit, empty())
	bup := infer unit
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

#if ALDORTEST
#include "types"
#pile
import from TestSubstitution
testExpression()
testSubst()
testUnify()
testBottomUp()
#endif
