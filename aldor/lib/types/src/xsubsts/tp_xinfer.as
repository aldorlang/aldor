#include "types"
#include "aldorio"
#pile

XBottomUp2: with
    bupAnnotations: Unit Expression -> Annotated(Expression, XTPoss)
    infer!: XInferState -> ()
== add
    import from XEnv
    import from List TypeTerm
    import from List Expression
    import from List SymbolMeaning
    import from List XTPoss
    import from Symbol
    import from XTPoss
    import from XTypes
    import from TypeTerm
    import from XSubstitution
    import from MachineInteger
    import from SymbolMeaningFields
    import from XInferUtils
    
    bupAnnotations(u: Unit Expression): Annotated(Expression, XTPoss) ==
        annotations(u, XTPoss, bupPhase$XInferState)

    infer!(state: XInferState): () ==
        initBup!(state)
	inferExpression(state, base state)

    inferCounter: CallCounter := counter("bupInfer", true)
    local inferExpression(state: XInferState, e: Expression): () ==
	pushEnv!(state, e)
	cc := open inferCounter
	lineWriter(cc) << e << newline
	inferExpression1(state, e)
	lineWriter(cc) << "bupInferExpression: " << e << " : " << annotation(state, e) << newline
	close(cc)
	popEnv!(state, e)
	
    local inferExpression1(state: XInferState, e: Expression): () ==
        term? e => inferTerm(state, e)
        apply? e => inferApply(state, e)
        comma? e => inferComma(state, e)
        lambda? e => inferLambda(state, e)
        if? e => inferIf(state, e)
	error "Missing infer"

    local inferTerm(state: XInferState, e: Expression): () ==
        import from List Expression
        sym: Symbol := term e
	ll: List XTPossItem := []
	for syme in symbolMeanings(env state, sym) repeat
	    if isVar? syme then
	        fvset: FVSet := fvset syme
	        ll := cons(item(type syme, empty fvset), ll)
	    else
	        ll := cons(item type syme, ll)
	annotate!(state, e, [i for i in ll])

#if 0
    local instantiate(tt: TypeTerm): (TypeTerm, XSubstitution) ==
        import from ListSet Symbol, XSubstitution, Expression, Fold2 FVSet
        constant? tt => (tt, empty())
	return (tt, empty())
	oldVars := vars tt
	newVars: List SymbolMeaning := [newVar()$XTypes for v in oldVars]
	xs := empty()
	for v in oldvars repeat
	    nvar := newVar()$XTypes
	    add!(xs, syme nvar, expr nvar) -- NB: (forall has to be annotated properly)
	    fv := fv + fvset syme nvar
        newExpr := expression(xs, expr tt)
	stdout << "instantiate " << tt << " --> " << newExpr << newline
	(constant newExpr, empty fv)
#endif

    local inferIf(state: XInferState, ifExpr: Expression): () ==
        inferExpression(state, ifTest ifExpr)
        inferExpression(state, ifPart ifExpr)
        inferExpression(state, ifElsePart ifExpr)

	tpTestPart := annotation(state, ifTest ifExpr)
	tpTestPart := intersect(tpTestPart, [boolType()])
	tpIfPart   := annotation(state, ifPart ifExpr)
	tpElsePart := annotation(state, ifElsePart ifExpr)

        stdout << "Bup If test " << tpTestPart << newline
        stdout << "Bup If if " << tpIfPart << newline
        stdout << "Bup If else " << tpElsePart << newline

	tpResult := intersect(tpIfPart, tpElsePart)
        stdout << "Bup If intersect " << tpResult << newline
	tpResult := constrain(tpResult, asConstraint tpTestPart)
        stdout << "Bup If merge " << tpResult << newline
	annotate!(state, ifExpr, tpResult)

    inferLambdaCounter: CallCounter := counter("bupInferLambda", true)

    local inferLambda(state: XInferState, e: Expression): () ==
        import from Fold2 ListSet Symbol, Fold2 XTPoss, List Expression
	makeLambda(tpi: XTPossItem): XTPoss ==
	    args: Expression := lambdaArgs()
	    assert constant? typeTerm tpi -- if not, we have to lift args
	    stdout << "makeLambda " << args << newline
	    stdout << "makeLambda " << tpi << newline
	    constrain([constant apply(expr(-"map"), [args, expr typeTerm tpi])], asConstraint [tpi])
	lambdaArgs(): Expression == 
	    l: List Expression := []
	    local ti: Expression
	    for decl in lambdaVars e repeat
	        if term? decl then
		    ti := expr first meanings(env(state), term decl)
		else
		    stdout << "odd term " << decl << newline
		l := cons(ti, l)
            comma(l)
	cc := open inferLambdaCounter
        writer(cc) << "(inferLambda " << cc << " " << e << newline
	inferExpression(state, lambdaBody e)
	writer(cc) << "Body: " << annotation(state, lambdaBody e) << newline
	tposs: XTPoss := (union, empty())/(makeLambda(ptt) for ptt in annotation(state, lambdaBody e))
	annotate!(state, e, tposs)
        writer(cc) << " inferLambda " << cc << " " << e << ": " << tposs << ")" << newline
	close(cc)


    local inferApplyCounter: CallCounter := counter("bupInferApply", true)

    local inferApply(state: XInferState, app: Expression): () == 
    	cc := open(inferApplyCounter)
        writer(cc) << cc << "(bupInferApply " << cc << " " << app << newline
        inferExpression(state, applicationOp app)
	for arg in applicationArgs app repeat
	    inferExpression(state, arg)
	    
	maps := filterMaps(annotation(state, applicationOp app))
	stdout << "maps " << maps << newline
	maps := filterArgCount(maps, applicationArgC app)
	stdout << "maps - check " << maps << newline
        writer(cc) << cc << " bupInferApply Op: " << applicationOp app << ": " << annotation(state, applicationOp app) << newline
	for arg in applicationArgs app for n in 1.. repeat
            writer(cc) << cc << "   bupinferApply " << n << ": " << annotation(state, arg) << newline
	tp := filterOpArgs(maps, [annotation(state, arg) for arg in applicationArgs app])
	writer(cc) << cc << "Apply result " << tp << newline
	pp := project(mapRet)
	retTp: XTPoss := [pp(tpi) for tpi in tp]
	writer(cc) << cc << "Apply result " << retTp << newline
	close cc
	annotate!(state, app, retTp)

    -- sPoss: ArgType, tposs: mapArg
    -- result: merged
    local filter(sPoss: XTPoss, tPoss: XTPoss): XTPoss ==
        stdout << "Filter" << sPoss << newline
        stdout << "Filter" << tPoss << newline
        merge(sPoss, tPoss)
		
    local inferComma(state: XInferState, e: Expression): () == never

    local tpossMapArgN(tp: XTPoss, n: MachineInteger): XTPoss ==
        import from Fold2 XTPoss
	cc := asConstraint tp
        (union, empty())/(constrain([mapArgN(typeTerm tpi, n)], cc) for tpi in tp)

XInferUtils: with
    filterMaps: XTPoss -> XTPoss
    filterArgCount: (XTPoss, MachineInteger) -> XTPoss
    filterOpArgs: (XTPoss, List XTPoss) -> XTPoss
    filterRetType: (XTPoss, XTPoss) -> XTPoss
== add
    import from List SymbolMeaning
    import from TypeTerm
    import from XTypes
    import from SymbolMeaningFields
    import from XSubstitution
    import from MachineInteger
    
    filterMaps(tp: XTPoss): XTPoss ==
        ll: List XTPossItem := []
        for tpi in tp repeat
	    if map? typeTerm tpi then
	        ll := cons(tpi, ll)
	    -- handle case of 'T -> (map A B)'
	    if syme? typeTerm tpi and isVar? syme typeTerm tpi then
	        newMap := map(newVar(), newVar())
		xs2 := copy constraint tpi
		add!(xs2, syme typeTerm tpi, expr newMap)
		stdout << "New item " << item(newMap, xs2) << " xs " << constraint tpi << newline
		ll := cons(item(newMap, xs2), ll)
	[tpi for tpi in ll]

    filterRetType(tp: XTPoss, tpIn: XTPoss): XTPoss ==
        -- it would be nicer to implement this as 'merge(A -> R, tpi)'
	ll: List XTPossItem := []
	for tpiIn in tpIn repeat
            for tpi in tp repeat
	        r := merge(project(mapRet)(tpi), tpiIn)
	    	stdout << "filter ret type candiate " << tpi << newline
	    	stdout << "filter ret type constr " << tpiIn << newline
	    	stdout << "filter ret type result " << r << newline
	    	if not failed? r then ll := cons(item(typeTerm tpi, constraint r), ll)
	[generator ll]

    local map?(tpi: XTPossItem): Boolean ==
        import from Partial Expression
        if isVar? typeTerm tpi then
	    tp := data(constraint tpi, syme typeTerm tpi)
	    tp and map? constant retract tp
	else
	    map? typeTerm tpi

    filterArgCount(tp: XTPoss, n: MachineInteger): XTPoss ==
        ll: List XTPossItem := []
        for tpi in tp repeat
	    -- handle case of 'T -> (map (comma N) B)'
	    if map? typeTerm tpi and mapArgC typeTerm tpi = n then
	        ll := cons(tpi, ll)
	[tpi for tpi in ll]

    filterOpArgs(tp: XTPoss, argPossl: List XTPoss): XTPoss ==
        import from Fold2 XTPoss
	stdout << "FilterOpArgs - tp   " << tp << newline
	stdout << "FilterOpArgs - args " << argPossl << newline
	res: XTPoss := empty()
	for mapTpi in tp repeat
	    cs: XConstraintSet := one(constraint mapTpi)
	    for argPossN in argPossl for n in 1 .. while not failed? cs repeat
	        stdout << "FilterOpArgs - case " << mapTpi << " " << argPossN << newline
		mapTpiN := mapArgN(typeTerm mapTpi, n)
                argM := merge(argPossN, constrain([item(mapTpiN, constraint mapTpi)], cs))
		cs := merge(cs, asConstraint argM)
	    stdout << "FilterOpArgs - Post " << cs << newline
            if not failed? cs then
	        res := union(constrain([mapTpi], cs), res)
	res


XTopDown2: with
    tdnAnnotations: Unit Expression -> Annotated(Expression, TypeTerm)
    infer!: (XInferState, TypeTerm) -> ()
    phase: NamedAnnotation(Expression, TypeTerm)
== add
    import from Expression
    import from List Expression
    import from MachineInteger
    import from ParamTypeTerm
    import from XTPoss
    import from TypeSatisfier
    import from TypeTerm
    import from XInferUtils
    import from XTypes    

    phase: NamedAnnotation(Expression, TypeTerm) == new "tdn"

    tdnAnnotations(u: Unit Expression): Annotated(Expression, TypeTerm) ==
        annotations(u, TypeTerm, phase)

    infer!(state: XInferState, top: TypeTerm): () ==
        initTdn!(state)
	inferExpression(state, base state, item(top))

    inferCounter: CallCounter := counter("inferTdn", true)

    local inferExpression(state: XInferState, e: Expression, tpi: XTPossItem): () ==
        aux(): () ==
            if? e => inferIfExpression(state, e, tpi)
            term? e => inferTermExpression(state, e, tpi)
	    apply? e => inferApplyExpression(state, e, tpi)
	    lambda? e => inferLambdaExpression(state, e, tpi)
	    def? e => inferDefineExpression(state, e, tpi)
	    error("tdn case missing for " + toString e)
	cc := open inferCounter
	writer(cc) << "(infer " << cc << " " << e << ": " << tpi << newline
	aux()
	writer(cc) << " infer " << cc << " " << e << ": " << type(state, e) << ")" << newline
	close cc

    local inferIfExpression(state: XInferState, e: Expression, tpi: XTPossItem): () ==
        -- test must be of type Bool
	-- parts must be of type 'tf' & unify correctly
	poss: XTPoss := annotation(state, e)
	filtered: XTPoss := merge(poss, [tpi])
	stdout << "Filtered: " << filtered << newline
	if not unique? filtered then
	    markError(state, e, "If with non-constant tdn type")
	else
	    tFinal := unique(filtered)
	    inferExpression(state, ifTest e, item(boolType(), constraint tFinal))
	    inferExpression(state, ifPart e, tFinal)
	    inferExpression(state, ifElsePart e, tFinal)
	    type!(state, e, expand tFinal)

    inferLambdaCounter: CallCounter := counter("tdnInferLambda", true)
    local inferLambdaExpression(state: XInferState, e: Expression, tpi: XTPossItem): () ==
        cc := open(inferLambdaCounter)
        writer(cc) << "(" << cc << " inferLambda " << e << " " << tpi << newline
	poss: XTPoss := annotation(state, e)
        writer(cc) << " " << cc << " " << poss << newline
	poss := merge(poss, [tpi])
	not unique? poss =>
	    markError(state, e, "Failed to infer lambda")
	inferExpression(state, lambdaBody e, project(mapRet)(unique poss))
        writer(cc) << " inferLambda " << cc << " " << e << ": " << unique poss << ")" << newline
	type!(state, e, expand unique poss)

    local inferApplyExpression(state: XInferState, e: Expression, tpi: XTPossItem): () ==
        import from List XTPossItem, XConstraintSet
        -- 1. Find appropriate function types
	stdout << "TPoss in " << tpi << newline
	topPoss := merge(annotation(state, e), [tpi])
	stdout << "Top Poss " << topPoss << newline
	op := applicationOp e
	mapTPoss := annotation(state, op)
	stdout << "TPoss in " << mapTPoss << newline
	mapTPoss := filterMaps(mapTPoss)
	stdout << "As maps " << mapTPoss << newline
	mapTPoss: XTPoss := filterRetType(mapTPoss, topPoss)
	stdout << " .. filter Ret " << mapTPoss << newline
	mapTPoss := filterArgCount(mapTPoss, applicationArgC e)
	stdout << " .. Filter Count " << mapTPoss << newline
	empty? mapTPoss => markError(state, e, "No applicable meaning for map type")
	stdout << "Post filter args " << mapTPoss << newline
	-- Hope that there's a unique type left
	not unique? mapTPoss =>
	    stdout << "not unique " << mapTPoss << newline
	    markError(state, e, "Operator not unique")
	inferExpression(state, op, unique mapTPoss)
	for l in project(mapArgs)(unique mapTPoss) for arg in applicationArgs e repeat
	    inferExpression(state, arg, l)
	type!(state, e, expand project(mapRet)(unique mapTPoss))

    local tdnApplyOpsCounter: CallCounter := counter("tdnApplyOpsCounter", false)

#if 0
    local inferApplyExpression1(state: XInferState, e: Expression, mapTfL: List Cross(Unifier, TypeTerm)): TypeTerm ==
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
#endif
    inferDefineCounter: CallCounter := counter("inferDefineTdn", true)
    local inferDefineExpression(state: XInferState, e: Expression, tpi: XTPossItem): () == never

    inferTermCounter: CallCounter := counter("inferTdn", true)
    local inferTermExpression(state: XInferState, e: Expression, tpi: XTPossItem): () ==
        -- 1. Find meaning of type 'tf'
	-- 2. set meaning on this bit of syntax
	-- 3. Mark with this type
	poss: XTPoss := annotation(state, e)
	filtered: XTPoss := merge(poss, [tpi])
	if empty? filtered then
	    markError(state, e, "No match")
	else if #filtered > 1 then
	    markError(state, e, "multiple matches")
        else
	    cc := open(inferTermCounter)
	    writer(inferTermCounter) << cc << " Infer Term " << e << " " << tpi << " --> " << filtered << newline
	    writer(inferTermCounter) << cc << " Infer Term - poss " << poss << newline
	    writer(inferTermCounter) << cc << " Infer Term - final " << (expand unique filtered)@TypeTerm << newline
	    close(cc)
	    type!(state, e, expand unique filtered)

    local tdnSatisfies?(s: TypeTerm, tpi: XTPossItem): SatResult == never
#if 0    
        stdout << "tdnSat: " << s << " sat " << t << newline
        satisfies?(s, t)
#endif

    local tdnFilter(s: TypeTerm, t: TypeTerm): Partial TypeTerm ==
        stdout << "tdnSat: " << s << " sat " << t << newline
        satisfies?(s, t) => [s]
	failed

    local markError(state: XInferState, e: Expression, msg: String): () ==
        txt := msg + ": " + toString e
        error txt
#if 0    
    filterRetCounter: CallCounter := counter("filterRet", false)
    local filterRet(opXTPoss: XTPoss, tf: TypeTerm): List Cross(Unifier, TypeTerm) ==
        import from List Unifier
        cc := open filterRetCounter
        l: List Cross(Unifier, TypeTerm) := []
	writer(cc) << "(filterRet " << cc << " " << opXTPoss << " " << tf << newline
	for opTPP in opXTPoss repeat
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
    filterParams(state: XInferState, e: Expression, mapTfL: List TypeTerm): List TypeTerm ==
        writer(inferCounter) << "filter " << e << " tfls " << mapTfL << newline
        mapTfL
#endif
XCompiler2: with
    compile: (XEnv, Expression, top: TypeTerm == unknown(), stages: 'ALL,BUP' == ALL) -> Unit Expression
    export from 'ALL,BUP'
== add
    import from XScopeBind
    import from Annotated(Expression, XTPoss)
    import from Annotated(Expression, TypeTerm)

    compile(env: XEnv, expr: Expression, top: TypeTerm, stages: 'ALL,BUP'): Unit Expression ==
        unit: Unit Expression := new(expr)
	bind(unit, env)
        envAnnotations: Annotated(Expression, XEnv) := annotations(unit)$XScopeBind

        rootEnv: XEnv := annotation(envAnnotations, base unit)
	state: XInferState := init(unit, rootEnv)

	stdout << "Bottom up " << expr << newline
	infer!(state)$XBottomUp2
	stdout << "Bottom Up Result " << annotations(state) << newline
	if stages = ALL then
	    stdout << "Top Down " << expr << newline
	    infer!(state, top)$XTopDown2
	    stdout << "Top Down Result " << types(state) << newline
	return unit

XTestInfer: with
    simpleTest: () -> ()
== add
    import from Symbol, XCompiler2, TypeTerm, XTPoss, XTopDown2
    import from Annotated(Expression, XTPoss)
    import from Annotated(Expression, TypeTerm)
    import from Assert XTPoss
    import from Assert TypeTerm
    import from MachineInteger
    
    local bupCompile(env: XEnv, e: Expression): Unit Expression ==
        compile(env, e, stages == BUP)
    
    local tdnCompile(env: XEnv, e: Expression, top: TypeTerm): Unit Expression ==
        compile(env, e, top)

    simpleTest(): () ==
        test("testBupVar1", testBupVar1)
	test("testBupIf1", testBupIf1)
	test("testBupIf2", testBupIf2)
	test("testBupIf3", testBupIf3)
	test("testBupApply1", testBupApply1)
	test("testBupApply2", testBupApply2)

	test("testBottomUpParamApply0", testBottomUpParamApply0)

	test("testBupLambda1", testBupLambda1)
	test("testBupLambda2", testBupLambda2)
	
	test("testTdnVar", testTdnVar)
	test("testTdnIf", testTdnIf)
	test("testTdnIf2", testTdnIf2)
        test("testTdnApply1", testTdnApply1)
        test("testTdnApply2", testTdnApply2)
        test("testTdnApply3", testTdnApply3)
        test("testTdnApply4", testTdnApply4)
        test("testTdnLambda1", testTdnLambda1)
        test("testTdnLambda2", testTdnLambda2)

    local test(s: String, f: () -> ()): () ==
        stdout << "------------- " << newline
        stdout << "Start: " << s << newline
	f()
        stdout << "Done: " << s << newline
        stdout << "------------- " << newline

    -- foo: int, foo: string => foo : [int, string]
    local testBupVar1(): () == 
        env: XEnv := empty()
	put!(env, -"foo", fromString "int")
	put!(env, -"foo", fromString "string")

        expr: Expression := fromString "foo"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2

	assertEquals([fromString "string", fromString "int"], annotation(bup, expr))

    testBupApply1(): () ==
        env: XEnv := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"nil", fromString "(apply list int)")
	put!(env, -"cons", fromString "(apply map (comma int (apply list int)) (apply list int)))")
    
	expr: Expression := fromString "(apply cons one nil)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBupApply2(): () ==
        env: XEnv := empty()
	put!(env, -"f", fromString "(apply map (comma int) int)")
	put!(env, -"f", fromString "(apply map (comma bool) str)")
	put!(env, -"one", fromString "int")
	expr: Expression := fromString "(apply f one)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([intType()$XTypes], annotation(bup, expr))

    testBottomUpParamApply0(): () ==
        env: XEnv := empty()
	put!(env, -"one", fromString "int")
	put!(env, -"empty", fromString "(forall (t) (apply map (comma) (apply list t)))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
    
	expr: Expression := fromString "(apply cons one (apply empty))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBupIf1(): () ==
        env: XEnv := empty()
	put!(env, -"tval", fromString "int")
	put!(env, -"fval", fromString "int")
	put!(env, -"true", fromString "bool")
    
	expr: Expression := fromString "(if true tval fval)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "int"], annotation(bup, expr))

    testBupIf2(): () ==
        env: XEnv := empty()
	vType := newVar()$XTypes
	put!(env, -"tval", fromString "int")
	put!(env, -"fval", fromString "int")
	put!(env, -"v", vType)
    
	expr: Expression := fromString "(if v tval fval)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "int"], annotation(bup, expr))
	assertEquals([vType], annotation(bup, ifTest expr))

    testBupIf3(): () ==
        env: XEnv := empty()
	vType := newVar()$XTypes
	put!(env, -"v", vType)
	xType := newVar()$XTypes
	put!(env, -"x", xType)
    
	expr: Expression := fromString "(if v v x)"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "bool"], annotation(bup, expr))

    testBupLambda1(): () ==
        env: XEnv := empty()
	put!(env, -"plus", fromString "(apply map (comma int int) int)")
	put!(env, -"one", fromString "int")
    
	expr: Expression := fromString "(lambda (x) (apply plus x one))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

    testBupLambda2(): () ==
        env: XEnv := empty()
	put!(env, -"one", fromString "int")
    
	expr: Expression := fromString "(lambda (f) (apply f (apply f one)))"
	unit: Unit Expression := bupCompile(env, expr)
	bup := bupAnnotations(unit)$XBottomUp2
	-- lambda takes a fn, and applies it to 'one' twice
	-- has type (int -> int) -> int
	assertEquals([fromString "(apply map (comma (apply map (comma int) int)) int)"], annotation(bup, expr))

    testTdnVar(): () ==
        env: XEnv := empty()
	xType := newVar()$XTypes
	put!(env, -"x", xType)
    
	expr: Expression := fromString "x"
	unit: Unit Expression := tdnCompile(env, expr, boolType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(boolType()$XTypes, annotation(tdn, expr))

    testTdnIf(): () ==
        env: XEnv := empty()
	xType := newVar()$XTypes
	yType := newVar()$XTypes
	put!(env, -"x", xType)
	put!(env, -"y", yType)
    
	expr: Expression := fromString "(if x x y)"
	unit: Unit Expression := tdnCompile(env, expr, boolType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(boolType()$XTypes, annotation(tdn, expr))
	assertEquals(boolType()$XTypes, annotation(tdn, ifPart(expr)))
	assertEquals(boolType()$XTypes, annotation(tdn, ifElsePart(expr)))

    testTdnIf2(): () ==
        env: XEnv := empty()
	xType := newVar()$XTypes
	yType := newVar()$XTypes
	put!(env, -"x", xType)
	put!(env, -"y", yType)
	put!(env, -"one", fromString "int")
	put!(env, -"v", fromString "int")
	put!(env, -"v", fromString "str")
    
	expr: Expression := fromString "(if x one v)"
	unit: Unit Expression := tdnCompile(env, expr, intType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(boolType()$XTypes, annotation(tdn, ifTest(expr)))
	assertEquals(intType()$XTypes, annotation(tdn, ifPart(expr)))
	assertEquals(intType()$XTypes, annotation(tdn, ifElsePart(expr)))
	
    testTdnApply1(): () ==
        env: XEnv := empty()
	put!(env, -"f", fromString "(apply map (comma int) int)")
	put!(env, -"f", fromString "(apply map (comma bool) str)")
	put!(env, -"one", fromString "int")
	expr: Expression := fromString "(apply f one)"
	unit: Unit Expression := tdnCompile(env, expr, intType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(intType()$XTypes, annotation(tdn, applicationArgN(expr, 1)))
	assertEquals(fromString("(apply map (comma int) int)"), annotation(tdn, applicationOp(expr)))
	
    testTdnApply2(): () ==
        env: XEnv := empty()
	xType := newVar()$XTypes
	put!(env, -"f", fromString "(apply map (comma int) int)")
	put!(env, -"f", fromString "(apply map (comma bool) str)")
	put!(env, -"x", xType)
	expr: Expression := fromString "(apply f x)"
	unit: Unit Expression := tdnCompile(env, expr, intType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(intType()$XTypes, annotation(tdn, applicationArgN(expr, 1)))
	assertEquals(fromString("(apply map (comma int) int)"), annotation(tdn, applicationOp(expr)))

    testTdnApply3(): () ==
        import from TypeTerm, List Expression
        env: XEnv := empty()
	(xType, yType) := (newVar()$XTypes, newVar()$XTypes)
	xExpr: Expression := expr(xType)$TypeTerm
	mapXtoX: TypeTerm := constant apply(expr(-"map"), [comma [xExpr], xExpr])
	put!(env, -"f", mapXtoX)
	put!(env, -"x", xType)

	expr: Expression := fromString "(apply f x)"
	unit: Unit Expression := tdnCompile(env, expr, intType()$XTypes)
	tdn := tdnAnnotations(unit)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(intType()$XTypes, annotation(tdn, applicationArgN(expr, 1)))

    testTdnApply4(): () ==
        import from TypeTerm, List Expression
        env: XEnv := empty()
	(xType, fType) := (newVar()$XTypes, newVar()$XTypes)
	put!(env, -"f", fType)
	put!(env, -"x", xType)

	expr: Expression := fromString "(apply f (apply f x))"
	unit: Unit Expression := tdnCompile(env, expr, intType()$XTypes)
	tdn := tdnAnnotations(unit)
	exprArg1 := applicationArgN(expr, 1)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(intType()$XTypes, annotation(tdn, exprArg1))
	assertEquals(fromString "(apply map (comma int) int)", annotation(tdn, applicationOp(expr)))
	assertEquals(fromString "(apply map (comma int) int)", annotation(tdn, applicationOp(exprArg1)))

    testTdnLambda1(): () ==
        import from TypeTerm, List Expression
        env: XEnv := empty()
	(xType, yType) := (newVar()$XTypes, newVar()$XTypes)
	put!(env, -"plus", fromString "(apply map (comma int int) int)")
	put!(env, -"one", intType()$XTypes)

	expr: Expression := fromString "(lambda (x) (apply plus x one))"
	unit: Unit Expression := tdnCompile(env, expr, fromString "(apply map (comma int) int)")
	tdn := tdnAnnotations(unit)
	assertEquals(fromString "(apply map (comma int) int)", annotation(tdn, expr))
	assertEquals(intType()$XTypes, annotation(tdn, lambdaBody expr))

    testTdnLambda2(): () ==
        import from TypeTerm, List Expression
        env: XEnv := empty()
	(xType, yType) := (newVar()$XTypes, newVar()$XTypes)
	put!(env, -"len", fromString "(apply map (comma int) int)")
	put!(env, -"len", fromString "(apply map (comma str) int)")
	put!(env, -"one", intType()$XTypes)

	expr: Expression := fromString "(apply (lambda (x) (apply len x)) one)"
	unit: Unit Expression := tdnCompile(env, expr, fromString "int")
	tdn := tdnAnnotations(unit)
	assertEquals(intType()$XTypes, annotation(tdn, expr))
	assertEquals(fromString "(apply map (comma int) int)", annotation(tdn, applicationOp expr))

#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from XTestInfer
simpleTest()
stdout << "DONE!" << newline
#endif

#if 0

cpsFib(n: Int, c: Int -> Int): Int ==
    if n = 0 then c(0) else cpsFib(n-1, Lx.n*c(n-1))

#endif
