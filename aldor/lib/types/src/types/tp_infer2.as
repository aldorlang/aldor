#include "types"
#include "aldorio"
#pile

BottomUp2: with
    bupAnnotations: Unit Expression -> Annotated(Expression, TPoss)
    infer!: InferState -> ()
== add
    import from Env
    import from Expression
    import from Fold2 ListSet Symbol
    import from List TypeTerm
    import from List Expression
    import from List TPoss
    import from MachineInteger
    import from ParamTypeTerm
    import from ParamSet
    import from Partial TypeTerm
    import from Substitution
    import from Symbol
    import from TPoss
    import from TypeTerm
    import from TypeTermUtils
    import from UnificationTools2
    import from Unifier
    import from UnifierPair

    bupAnnotations(u: Unit Expression): Annotated(Expression, TPoss) ==
        annotations(u, TPoss, bupPhase$InferState)

    infer!(state: InferState): () ==
        initBup!(state)
	inferExpression(state, base state)

    inferCounter: CallCounter := counter("bupInfer", true)
    local inferExpression(state: InferState, e: Expression): () ==
	pushEnv!(state, e)
	cc := open inferCounter
	lineWriter(cc) << e << newline
	inferExpression1(state, e)
	lineWriter(cc) << e << " : " << annotation(state, e) << newline
	close(cc)
	popEnv!(state, e)
	
    local inferExpression1(state: InferState, e: Expression): () ==
        term? e => inferTerm(state, e)
        apply? e => inferApply(state, e)
        comma? e => inferComma(state, e)
        lambda? e => inferLambda(state, e)
        if? e => inferIf(state, e)
	error "Missing infer"

    local inferTerm(state: InferState, e: Expression): () ==
        import from List Expression
	instantiate(t: TypeTerm): ParamTypeTerm ==
            stdout << "instantiating " << t << " " << param? expr t << newline
	    constant? t => asParam t
	    param? expr t => -- (forall (tvar) (param var tvar))
	        stdout << "PP: " << expr t << " - " << param? expr t << " " << " " << parts expr t << newline
	        tvar := unique vars t
		var: Symbol := paramVar expr t
		newvar1 := new()$Symbol
		withParams(asParam forall(newvar1, expr newvar1), param(var, constant expr newvar1)@ParamSet)
	    newVars: ListSet Symbol := [new()$Symbol for oldvar in vars t]
	    newSubst := sigma((oldvar, expr(newvar)$Expression) for oldvar in vars t for newvar in newVars)
	    asParam forall(newVars, apply(newSubst, expr(t)$TypeTerm))
        sym: Symbol := term e
	annotate!(state, e, [instantiate t for t in meanings(env(state), sym)])

    local inferIf(state: InferState, ifExpr: Expression): () ==
        inferExpression(state, ifTest ifExpr)
        inferExpression(state, ifPart ifExpr)
        inferExpression(state, ifElsePart ifExpr)
	poss: List Unifier := []
	for ptt in annotation(state, ifTest ifExpr) repeat
	    ur: UnifierPair := unify(ptt, asParam fromString "bool")
	    if not failed? ur then
	        poss := cons(left ur, poss)
	tposs: List ParamTypeTerm := []
	for tParams in poss repeat
	    for pttIfPart in annotation(state, ifPart ifExpr) repeat
		for pttElsePart in annotation(state, ifElsePart ifExpr) repeat
		    u2 := unify(pttIfPart, pttElsePart)
		    if not failed? u2 then
		        u := merge(tParams, left u2)
			stdout << " InferIf - test: " << tParams << newline
			stdout << " InferIf - ifPart: " << pttIfPart << newline
			stdout << " InferIf - elsePart: " << pttElsePart << newline
			stdout << " InferIf - if/Else: " << u2 << newline
			stdout << " InferIf - merge: " << u << newline
			stdout << " InferIf - result: " << apply(u, pttIfPart) << newline
		        tposs := cons(apply(u, pttIfPart), tposs)
	annotate!(state, ifExpr, [ ptt for ptt in tposs])

    inferLambdaCounter: CallCounter := counter("bupInferLambda", false)

    local inferLambda(state: InferState, e: Expression): () ==
        cc := open(inferLambdaCounter)
        matchParam(sym: Symbol, ps: ParamSet): TypeTerm ==
	    ssym := paramType(env state, sym)
	    ptt := find(ssym, ps)
	    writer(cc) << "match " << cc << " " << ps << ": " << ssym << " -> " << ptt << newline
	    not failed? ptt => forall(vars retract ptt, paramExpr expr retract ptt)
	    forall(newsym, expr newsym) where newsym := new()$Symbol
	lambdaVarType(decl: Expression, ps: ParamSet): TypeTerm ==
	    declare? decl => constant declareType decl
	    term? decl => matchParam(term decl, ps)
	    error "bad param"
        paramVar(decl: Expression): ListSet Symbol ==
	    declare? decl => []
	    term? decl => [paramType(env state, term decl)]
	    error "bad param - paramVar"
	lambdaParam?(sym: Symbol): Boolean ==
	    stdout << "lambdaParam?: " << sym << " in " << varSyms << newline
	    member?(sym, varSyms)
	removeParams(ps: ParamSet): ParamSet == [p for p in ps | not lambdaParam?(paramVar expr p)]
	--removeParams(ps: ParamSet): ParamSet == ps
	makeLambda(retType: ParamTypeTerm): ParamTypeTerm ==
	    varTypes: List TypeTerm := [lambdaVarType(decl, paramSet retType) for decl in lambdaVars e]
	    params: ParamSet := [p for p in paramSet retType | not member?(paramVar expr p, varSyms)]
	    mapExpr: Expression := apply(expr(-"map"), [comma [expr var for var in varTypes], expr typeTerm retType])
	    withParams(withType(retType, forall(fv retType, mapExpr)), params)
        writer(cc) << "(inferLambda " << cc << " " << e << newline
	varSyms: ListSet Symbol := (+,[])/(paramVar decl for decl in lambdaVars e)
	inferExpression(state, lambdaBody e)
	tposs: TPoss := [ makeLambda(ptt) for ptt in annotation(state, lambdaBody e)]
	annotate!(state, e, tposs)
        writer(cc) << " inferLambda " << cc << " " << e << ": " << tposs << ")" << newline
	close(cc)

    inferApplyCounter: CallCounter := counter("bupInferApply", false)

    local inferApply(state: InferState, app: Expression): () ==
    	cc := open(inferApplyCounter)
        writer(cc) << "(bupInferApply " << cc << " " << app << newline
        inferExpression(state, applicationOp app)
	for arg in applicationArgs app repeat
	    inferExpression(state, arg)
	maps := filterMaps(annotation(state, applicationOp app))
	tp := filterOps(maps, [annotation(state, arg) for arg in applicationArgs app])
        writer(cc) << " bupInferApply " << applicationOp app << ": " << annotation(state, applicationOp app) << newline
	for arg in applicationArgs app for n in 1.. repeat
            writer(cc) << " bupinferApply " << n << ": " << annotation(state, arg) << newline
        writer(cc) << " bupInferApply " << cc << " --> " << tp << ")" << newline
	annotate!(state, app, tp)

    local inferComma(state: InferState, e: Expression): () == never

    filterOpsCounter: CallCounter := counter("bupFilterOps", false)

    local filterOps(maps: TPoss, argPoss: List TPoss): TPoss ==
        cc := open filterOpsCounter
	possR: List ParamTypeTerm := []
	-- for (mapType, mapParams) in maps repeat
	for pttMap in maps repeat
	    possl: List Unifier := [ empty() ]
	    for argNPoss in argPoss for i in 1.. repeat
	        newPossl: List Unifier := []
	        mapArgPTT := mapArgN(pttMap, i)
	        writer(cc) << "(Argument - fn " << cc << " " << i << " = " << mapArgPTT << newline
	        writer(cc) << " Argument - tx " << cc << " " << i << " poss " << argNPoss << newline
	        writer(cc) << " poss in " << cc << " " << i << " poss " << # possl << newline
		for argPTT in argNPoss for j in 1.. repeat
	            writer(cc) << " Argument " << cc << " " << i << "," << j << " test " << argPTT << newline
		    ur := unify(mapArgPTT, argPTT)
	            writer(cc) << " Argument " << cc << " " << i << "," << j << " test --> " << ur << newline
		    for poss in possl repeat
	                writer(cc) << " Argument " << cc << " " << i << "," << j << " poss " << left ur << " <<-->> " << poss << newline
			u := merge(left ur, poss)
	                writer(cc) << " Argument " << cc << " --> " << i << "," << j << " merged " << u << newline
			if not failed? u then
			    writer(cc) << "  adding " << u << newline
			    newPossl := cons(u, newPossl)
		possl := newPossl
	        writer(cc) << " Argument complete: " << possl << newline
	        writer(cc) << " Argument complete: " << cc << " " << i << ")" << newline
	    stdout << "Final for " << pttMap << " " << possl << newline
	    possR := append!([apply(possN, mapRet pttMap) for possN in possl], possR)
	finalTp: TPoss := [p for p in possR]
	close cc
	return finalTp

    local filterMaps(tp: TPoss): TPoss == [t for t in tp | map? t]

TopDown2: with
    tdnAnnotations: Unit Expression -> Annotated(Expression, TypeTerm)
    infer!: (InferState, TypeTerm) -> ()
    phase: NamedAnnotation(Expression, TypeTerm)
== add
    import from Expression
    import from List Expression
    import from MachineInteger
    import from ParamTypeTerm
    import from TPoss
    import from TypeSatisfier
    import from TypeTerm
    import from TypeTermUtils
    import from UnificationTools2
    import from Unifier
    import from UnifierPair
    
    phase: NamedAnnotation(Expression, TypeTerm) == new "tdn"

    tdnAnnotations(u: Unit Expression): Annotated(Expression, TypeTerm) ==
        annotations(u, TypeTerm, phase)

    infer!(state: InferState, top: TypeTerm): () ==
        initTdn!(state)
	topTf := typeTerm unique annotation(state, base state)
	inferExpression(state, base state, top)

    inferCounter: CallCounter := counter("inferTdn", false)

    local inferExpression(state: InferState, e: Expression, tf: TypeTerm): () ==
        aux(): () ==
            if? e => inferIfExpression(state, e, tf)
            term? e => inferTermExpression(state, e, tf)
	    apply? e => inferApplyExpression(state, e, tf)
	    lambda? e => inferLambdaExpression(state, e, tf)
	    error("tdn case missing for " + toString e)
	cc := open inferCounter
	writer(cc) << "(infer " << cc << " " << e << ": " << tf << newline
	aux()
	writer(cc) << " infer " << cc << " " << e << ": " << type(state, e) << ")" << newline
	close cc
	
    local inferIfExpression(state: InferState, e: Expression, tf: TypeTerm): () ==
        -- test must be of type Bool
	-- parts must be of type 'tf' & unify correctly
	poss: TPoss := annotation(state, e)
	filtered: TPoss := find(poss, tf, tdnSatisfies?)
	if not unique? filtered then
	    markError(state, e, "If with non-constant tdn type")
	else
	    tFinal := typeTerm unique(filtered)
	    inferExpression(state, ifTest e, fromString "bool")
	    inferExpression(state, ifPart e, tFinal)
	    inferExpression(state, ifElsePart e, tFinal)
	    type!(state, e, tFinal)

    local inferLambdaExpression(state: InferState, e: Expression, tf: TypeTerm): () ==
    	local sat?(t1: TypeTerm, t2: TypeTerm): SatResult == satisfies?(t1, t2)
        filteredPoss := find(annotation(state, e), tf, sat?)
        not unique? filteredPoss => error "Multiple meanings for lambda"
	stdout << "Final lambda tposs " << filteredPoss << newline
        finalTf := unique filteredPoss
	
	inferExpression(state, lambdaBody e, typeTerm mapRet finalTf)
	-- find type assignment in tposs
	-- add types of parameters to environment

	type!(state, e, typeTerm finalTf)

    local inferApplyExpression(state: InferState, e: Expression, tf: TypeTerm): () ==
        -- 1. Find appropriate function types
	topPoss := find(annotation(state, e), tf, tdnSatisfies?)
	op := applicationOp e
	mapTfL: List Cross(Unifier, TypeTerm) := filterRet(annotation(state, op), tf)
	empty? mapTfL => markError(state, e, "No applicable meaning for map type")

	finalTf := inferApplyExpression1(state, e, mapTfL)	

	type!(state, e, finalTf)

    local tdnApplyOpsCounter: CallCounter := counter("tdnApplyOpsCounter", false)

    local inferApplyExpression1(state: InferState, e: Expression, mapTfL: List Cross(Unifier, TypeTerm)): TypeTerm ==
        cc := open tdnApplyOpsCounter

        local consumeArgument(opTf: TypeTerm, n: MachineInteger, u: Unifier): Unifier ==
	    arg := (applicationArgs e).n
	    tfArg := unique annotation(state, arg)
	    u1: UnifierPair := unify(mapArgN(opTf, n), typeTerm tfArg)
	    stdout << cc << " Arg Unify (whole) " << n << ": " << opTf << newline
	    stdout << cc << " Arg Unify (tfArg) " << n << ": " << mapArgN(opTf, n) << newline
	    stdout << cc << " Arg Unify (arg) " << n << ": " << tfArg << newline
	    stdout << cc << " Arg Unify --> " << u1 << newline
	    u2 := merge(left u1, u)
	    stdout << cc << " Arg merged --> " << u2 << newline
	    return u2
	    
	ll: List Cross(Unifier, TypeTerm) := []
        for (u, tt) in mapTfL repeat
	    uN := u
	    for i in 1..#(applicationArgs e) while not failed? uN repeat
	        uN := consumeArgument(tt, i, uN)
	    not failed? uN => ll := cons( (uN, tt), ll)

	if # ll ~= 1 then
	    import from List Unifier
	    stdout << "tposs - tf " << [y for (x, y) in ll] << newline
	    stdout << "tposs - uu" << [x for (x, y) in ll] << newline
	    markError(state, e, "failed to find match")
	    return unknown()

	(uFinal, tt) := first ll
	finalTfOp := uFinal tt
	stdout << cc << " Final Unifer --> " << uFinal << newline
	stdout << cc << " Final Op --> " << finalTfOp << newline

	-- recurse..	
	inferExpression(state, applicationOp e, finalTfOp)
	for i in 1..mapArgC finalTfOp repeat
	    finalTfArg := mapArgN(finalTfOp, i)
	    inferExpression(state, apply(applicationArgs e, i), finalTfArg)
	close cc
	return mapRet finalTfOp


    local inferTermExpression(state: InferState, e: Expression, tf: TypeTerm): () ==
        -- 1. Find meaning of type 'tf'
	-- 2. set meaning on this bit of syntax
	-- 3. Mark with this type
	poss: TPoss := annotation(state, e)
	filtered: List TypeTerm := [typeTerm ptt for ptt in find(poss, tf, tdnSatisfies?)]
	if empty? filtered then
	    markError(state, e, "No match")
	else if #filtered > 1 then
	    markError(state, e, "multiple matches")
        else
	    type!(state, e, first filtered)

    local tdnSatisfies?(s: TypeTerm, t: TypeTerm): SatResult ==
        stdout << "tdnSat: " << s << " sat " << t << newline
        satisfies?(s, t)

    local tdnFilter(s: TypeTerm, t: TypeTerm): Partial TypeTerm ==
        stdout << "tdnSat: " << s << " sat " << t << newline
        satisfies?(s, t) => [s]
	failed

    local markError(state: InferState, e: Expression, msg: String): () ==
        txt := msg + ": " + toString e
        error txt
    
    filterRetCounter: CallCounter := counter("filterRet", false)
    local filterRet(opTPoss: TPoss, tf: TypeTerm): List Cross(Unifier, TypeTerm) ==
        import from List Unifier
        cc := open filterRetCounter
        l: List Cross(Unifier, TypeTerm) := []
	writer(cc) << "(filterRet " << cc << " " << opTPoss << " " << tf << newline
	for opTPP in opTPoss repeat
	    flg := satisfies?(typeTerm mapRet opTPP, tf, allowUnify())
	    flg =>
	        u := unifier flg
	        c: Cross(Unifier, TypeTerm) := (left u, (left u) typeTerm opTPP)
	        if not failed? u then l := cons(c, l)
	writer(cc) << " filterRet - types " << cc << " " << [t for (ux, t) in l] << ")" << newline
	writer(cc) << " filterRet - unifiers" << cc << " " << [ux for (ux, t) in l] << ")" << newline
        close(cc)
	return l

    -- remove types that are excluded given the expression.
    -- this should deal with multiplicity, types outside arg range
    filterParams(state: InferState, e: Expression, mapTfL: List TypeTerm): List TypeTerm ==
        writer(inferCounter) << "filter " << e << " tfls " << mapTfL << newline
        mapTfL

TypeTermUtils: with
    map?: TypeTerm -> Boolean
    mapRet: TypeTerm -> TypeTerm
    mapArgC: TypeTerm -> MachineInteger
    mapArgN: (TypeTerm, MachineInteger) -> TypeTerm

    map?: ParamTypeTerm -> Boolean
    mapRet: ParamTypeTerm -> ParamTypeTerm
    mapArgC: ParamTypeTerm -> MachineInteger
    mapArgN: (ParamTypeTerm, MachineInteger) -> ParamTypeTerm
== add
    import from Symbol
    import from Expression
    import from TypeTerm
    import from List Expression

    map?(t: TypeTerm): Boolean == applyOf?(-"map", expr t)
    mapArgN(t: TypeTerm, n: MachineInteger): TypeTerm == 
        forall(vars t, apply(commaParts(first applicationArgs expr t), n))

    mapRet(t: TypeTerm): TypeTerm == forall(vars t, first rest applicationArgs expr t)
    mapArgC(t: TypeTerm): MachineInteger == # commaParts first applicationArgs expr t

    map?(t: ParamTypeTerm): Boolean == applyOf?(-"map", expr typeTerm t)
    mapRet(t: ParamTypeTerm): ParamTypeTerm == withParams(asParam mapRet(typeTerm t), paramSet t)
    mapArgC(t: ParamTypeTerm): MachineInteger == # commaParts first applicationArgs expr typeTerm t
    mapArgN(t: ParamTypeTerm, n: MachineInteger): ParamTypeTerm == withParams(asParam mapArgN(typeTerm t, n), paramSet t)

    local applyOf?(sym: Symbol, e: Expression): Boolean ==
        apply? e and term? applicationOp e and term applicationOp e = sym

Compiler2: with
    compile: (Env, Expression, top: TypeTerm == unknown(), stages: 'ALL,BUP' == ALL) -> Unit Expression
    export from 'ALL,BUP'
== add
    import from ScopeBind
    import from Annotated(Expression, TPoss)

    compile(env: Env, expr: Expression, top: TypeTerm, stages: 'ALL,BUP'): Unit Expression ==
        unit: Unit Expression := new(expr)
	bind(unit, env)
        envAnnotations: Annotated(Expression, Env) := annotations(unit)$ScopeBind

        rootEnv: Env := annotation(envAnnotations, base unit)
	state: InferState := init(unit, rootEnv)

	stdout << "Bottom up " << expr << newline
	infer!(state)$BottomUp2
	stdout << "Bottom up result " << annotations(state) << newline
	if stages = ALL then
	    stdout << "Top Down " << expr << newline
	    infer!(state, top)$TopDown2
	return unit


TestInfer: with
    testBottomUp: () -> ()
    testTopDown: () -> ()
== add
    import from Annotated(Expression, TPoss)
    import from Annotated(Expression, TypeTerm)
    import from Assert TPoss
    import from Assert TypeTerm	
    import from Compiler2
    import from List Expression
    import from ParamTypeTerm
    import from Symbol
    import from TPoss
    import from TypeTerm

    testBottomUpTmp(): () ==
        testBottomUpLambdaPair()

    testBottomUp(): () ==
        testBottomUpLambdaPair()
        testBottomUpVar1()
        testBottomUpIf1()
        testBottomUpIf2()
	testBottomUpApply0()
	testBottomUpApply1()
	testBottomUpApply2()
	testBottomUpApply3()
	testBottomUpApply4()
	testBottomUpLambda1()
	testBottomUpLambda2()
	testBottomUpLambda3()
	testBottomUpLambda4()
	testBottomUpLambdaIf1()
	testBottomUpLambdaIf1a()
	testBottomUpLambdaIf2()
	testBottomUpLambdaIf3()
	testBottomUpLambdaIf4()
	testBottomUpEmbeddedLambda0()
	testBottomUpEmbeddedLambda1()
	testBottomUpEmbeddedLambda2()
	
    testBottomUpVar1(): () ==
        env: Env := empty()
	put!(env, -"foo", fromString "int")
	put!(env, -"foo", fromString "string")

        expr: Expression := fromString "foo"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2

	assertEquals([fromString "string", fromString "int"], annotation(bup, expr))

    testBottomUpIf1(): () ==
        env: Env := empty()
	put!(env, -"any", fromString "(forall (x) x)")
	put!(env, -"one", fromString "int")
	put!(env, -"true", fromString "bool")

        expr: Expression := fromString "(if true any one)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2

	assertEquals([fromString "int"], annotation(bup, expr))

    testBottomUpIf2(): () ==
        env: Env := empty()
	put!(env, -"any", fromString "(forall (x) x)")

        expr: Expression := fromString "(if any any any)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(forall (x) x)"], annotation(bup, expr))

    testBottomUpLambda1(): () ==
        env: Env := empty()
	expr: Expression := fromString "(lambda ((: x int)) x)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

    testBottomUpLambda2(): () ==
        env: Env := empty()
	expr: Expression := fromString "(lambda (x) x)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(forall (k) (apply map (comma k) k))"], annotation(bup, expr))

    testBottomUpLambda3(): () ==
        env: Env := empty()
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
	put!(env, -"one", fromString "int")
	expr: Expression := fromString "(lambda (x) (apply cons one x))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply map (comma (apply list int)) (apply list int))"], annotation(bup, expr))

    testBottomUpLambda4(): () ==
        env: Env := empty()
	put!(env, -"pair", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	expr: Expression := fromString "(lambda (x) (apply pair x x))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(forall (k) (apply map (comma k) (apply pair k k)))"], annotation(bup, expr))

    testBottomUpLambdaIf1(): () ==
        env: Env := empty()
	expr: Expression := fromString "(lambda (x) (if x x x))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply map (comma bool) bool)"], annotation(bup, expr))

    testBottomUpApply0(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"empty", fromString "(forall (t) (apply map (comma) (apply list t)))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
    
	expr: Expression := fromString "(apply cons one (apply empty))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpApply1(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"nil", fromString "(forall (t) (apply list t))")
	put!(env, -"nil", fromString "string")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
    
	expr: Expression := fromString "(apply cons one nil)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpApply2(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"nil", fromString "(forall (t) (apply list t))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons nil nil)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(forall (x) (apply list (apply list x)))"], annotation(bup, expr))

    testBottomUpApply3(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"n", fromString "float")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply list float)", fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpApply4(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"s", fromString "string")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
	put!(env, -"bracket", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	
	expr: Expression := fromString "(apply bracket (apply cons n nil) s)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	assertEquals([fromString "(apply pair (apply list int) string)"], annotation(bup, expr))

    testBottomUpLambdaIf1a(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(forall (a) (apply map (comma a) bool))")
	put!(env, -"zero", fromString "int")
        expr: Expression := fromString "(lambda (x) (if (apply test x) x zero))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))


    testBottomUpLambdaIf2(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(apply map (comma int) bool)")
	put!(env, -"minus", fromString "(forall (a) (apply map (comma a) a))")
	put!(env, -"one", fromString "int")
	put!(env, -"zero", fromString "int")
        expr: Expression := fromString "(lambda (x) (if (apply test x) x (apply minus x)))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
        assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

    testBottomUpLambdaIf3(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(apply map (comma int) bool)")
        expr: Expression := fromString "(lambda (x y) (if (apply test x) x y))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
        assertEquals([fromString "(apply map (comma int int) int)"], annotation(bup, expr))

    testBottomUpLambdaIf4(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(forall (a) (apply map (comma a) bool))")
        expr: Expression := fromString "(lambda (x y) (if (apply test x) x y))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
        assertEquals([fromString "(forall (a) (apply map (comma a a) a))"], [tpp for tpp in annotation(bup, expr)])

    testBottomUpLambdaPair(): () ==
	env: Env := empty()
	put!(env, -"mkpair", fromString "(forall (a b) (apply map (comma a b) (apply pair a b) ))")
        expr: Expression := fromString "(lambda (x) (apply mkpair x x))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
        assertEquals([fromString "(forall (k) (apply map (comma k) (apply pair k k)))"], [tpp for tpp in annotation(bup, expr)])

    testBottomUpEmbeddedLambda0(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"foo", fromString "(forall (c) (apply map (comma c) c))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval foo one)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr))

    testBottomUpEmbeddedLambda1(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) x) one)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr))

    testBottomUpEmbeddedLambda2(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"mklist", fromString "(forall (x) (apply map (comma x) (apply list x)))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) (apply mklist x)) one)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$BottomUp2
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    local bupCompile(env: Env, e: Expression): Unit Expression ==
        compile(env, e, stages == BUP)

    -- Top Down tests
    testTopDown(): () ==
        testTopDownId()
        testTopDownIf()
	testTopDownApply()
	testTopDownApply2()
	testTopDownApply3()
	testTopDownApply4a()
	testTopDownApply4()
	testTopDownMap()
	testTopDownEmbeddedLambda1()
	testTopDownEmbeddedLambda2()
	testTopDownLambda()
	testTopDownLambda2()
	
    local testTopDownId(): () ==
	env: Env := empty()
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "one"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "int", annotation(tdn, expr))
        
    local testTopDownIf(): () ==
	env: Env := empty()
	put!(env, -"true", fromString "bool")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"some", fromString "(apply list int)")
        expr: Expression := fromString "(if true nil some)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))

    local testTopDownApply(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"list", fromString "(forall (x) (apply map (comma x) (apply list x)))")
	expr: Expression := fromString "(apply list one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))
	assertEquals(fromString "int", annotation(tdn, first applicationArgs expr))

    local testTopDownApply2(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (x) (apply map (comma x (apply list x)) (apply list x)))")
	expr: Expression := fromString "(apply cons one nil)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))
	assertEquals(fromString "int", annotation(tdn, first applicationArgs expr))
	assertEquals(fromString "(apply list int)", annotation(tdn, first rest applicationArgs expr))

    local testTopDownApply3(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"ten", fromString "int")
	put!(env, -"list", fromString "(forall (x) (apply map (comma x x) (apply list x)))")
	expr: Expression := fromString "(apply list one ten)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))
	assertEquals(fromString "int", annotation(tdn, first applicationArgs expr))
	assertEquals(fromString "int", annotation(tdn, first rest applicationArgs expr))


    local testTopDownApply4a(): () ==
        env: Env := empty()
	put!(env, -"f", fromString "(forall (x) (apply map (comma x x) int))")

	expr: Expression := fromString "(lambda (a b) (apply f a b))"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(forall (k) (apply map (comma k k) int))", annotation(tdn, expr))

    local testTopDownApply4(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
	--put!(env, -"f", fromString "(forall (x) (apply map (comma x (apply list x)) int))")
	put!(env, -"f", fromString "(forall (x) (apply map (comma x x) int))")

	expr: Expression := fromString "(apply (lambda (a b) (apply f a b)) one one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline

    local testTopDownApply5(): () ==
        env: Env := empty()
	put!(env, -"f", fromString "(forall (x) (apply map (comma x (apply list x)) int))")
	put!(env, -"f", fromString "(forall (x) (apply map (comma x x) int))")
	
    local testTopDownMap(): () ==
        env: Env := empty()
	put!(env, -"one", fromString "int")
        put!(env, -"ten", fromString "int")
        put!(env, -"add", fromString "(apply map (comma int int) int)")
        put!(env, -"range", fromString "(apply map (comma int int) (apply list int))")
        put!(env, -"mapcar", fromString "(forall (x y) (apply map (comma (apply list x) (apply map (comma x) y)) (apply list y)))")
        expr: Expression := fromString("(apply mapcar (apply range one ten) (lambda (x) (apply add x one)))") 
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "infer " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))

    testTopDownEmbeddedLambda1(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) x) one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	assertEquals(fromString "int", annotation(tdn, expr))

    testTopDownEmbeddedLambda2(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"mklist", fromString "(forall (x) (apply map (comma x) (apply list x)))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) (apply mklist x)) one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))

    testTopDownLambda(): () ==
	env: Env := empty()
	put!(env, -"plus", fromString "(apply map (comma int int) int)")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply (lambda (x) (apply plus x x)) one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline
	assertEquals(fromString "int", annotation(tdn, expr))
	xs := all(fromString "x", expr)
	for x in xs repeat
	    assertEquals(fromString "int", annotation(tdn, expr))

    testTopDownLambda2(): () ==
	env: Env := empty()
	put!(env, -"plus", fromString "(apply map (comma int int) int)")
	put!(env, -"plus", fromString "(apply map (comma float float) float)")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply (lambda (x) (apply plus x x)) one)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline
	assertEquals(fromString "int", annotation(tdn, expr))
	xs := all(fromString "x", expr)
	for x in xs repeat
	    assertEquals(fromString "int", annotation(tdn, expr))

#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestInfer
--testBottomUp()
--testTopDown()
stdout << "DONE!" << newline
#endif
