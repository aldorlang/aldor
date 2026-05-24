#include "types"
#include "aldorio"
#pile

BottomUp: with
    bupAnnotations: Unit Expression -> Annotated(Expression, TPoss)
    infer: Unit Expression -> Annotated(Expression, TPoss)
    phase: NamedAnnotation TPoss
== add
    import from List Expression, List TypeTerm
    import from List List TypeTerm
    import from List TPoss
    import from List Env
    import from ParamSet    
    import from ListSet Symbol
    import from Expression
    import from Substitution
    import from Symbol
    import from TPoss
    import from UnificationTools
    import from MachineInteger
    import from ListSet Symbol
    import from TypeTerm
    import from UnificationResult
    import from Fold2 ListSet Symbol
    import from Partial TypeTerm
    import from SExpression

    phase: NamedAnnotation TPoss == new "bup"

    bupAnnotations(u: Unit Expression): Annotated(Expression, TPoss) ==
        annotations(u, TPoss, phase)

    infer(unit: Unit Expression): Annotated(Expression, TPoss) ==
        envAnnotations: Annotated(Expression, Env) := annotations(unit)$ScopeBind
        rootEnv: Env := annotation(envAnnotations, base unit)
        bup := infer(envAnnotations, rootEnv, base unit)
	register(unit, TPoss, phase, bup)
	return bup
	
    local infer(envAnnotations: Annotated(Expression, Env), rootEnv: Env, expr: Expression): Annotated(Expression, TPoss) ==
        tbl: Annotated(Expression, TPoss) := new()
	envs: Record(l: List Env) := [[]]
	pushEnv!(e: Env): () ==
	    envs.l := cons(e, envs.l)
	popEnv!(): () ==
	    envs.l := rest envs.l
	env(): Env == first envs.l
	pushEnv! rootEnv
        inferExpression(e: Expression): () ==
	    stdout << "(infer " << e << " " << (if annotation?(envAnnotations, e) then "ENV" else "") << newline
	    if annotation?(envAnnotations, e) then
	        pushEnv!(annotation(envAnnotations, e))
	    inferExpression1 e
	    if annotation?(envAnnotations, e) then
	        popEnv!()
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
	    sym: Symbol := term termExpr
	    annotate!(tbl, termExpr, [instantiate t for t in meanings(env(), sym)])
	inferApply(app: Expression): () ==
	    inferExpression(applicationOp app)
	    for arg in applicationArgs app repeat
	        inferExpression(arg)
            tOp: TPoss := annotation(tbl, applicationOp app)
            maps := filterMaps(tOp)
	    annotate!(tbl, app, filterOps(tOp, [annotation(tbl, arg) for arg in applicationArgs app]))
	inferLambda(e: Expression): () ==
	    types := inferLambda1(env(), inferExpression, tbl, e)
	    annotate!(tbl, e, types)
	inferIf(app: Expression): () ==
	    inferExpression(ifTest app)
	    inferExpression(ifPart app)
	    inferExpression(ifElsePart app)
	    poss: List Cross(UnificationResult, ParamSet) := []
	    for (tt, subs) in annotation(tbl, ifTest app) repeat
	        ur := unify(tt, fromString "bool")
		if not failed? ur then
		   poss := cons((ur, subs), poss)
            tposs: List Cross(TypeTerm, ParamSet) := []
	    for (pp, tSubs) in poss repeat
	        for (ttIfPart, ifSubs) in annotation(tbl, ifPart app) repeat
	           (ur0, subs) := unifyParameters(tSubs, ifSubs)
		   stdout << "Unify parameters " << tSubs << " <> " << ifSubs << newline
		   stdout << "Unify parameters -> " << ur0 << " <> " << subs << newline
	           for (ttElse, elseSubs) in annotation(tbl, ifElsePart app) repeat
		       ttIfx := ur0 ttIfPart
		       ttElsex := ur0 ttElse
		       stdout << "if unify " << ttIfx << " <> " << ttElsex << newline
		       stdout << "if unify params " << ifSubs << " <> " << elseSubs << newline
		       ur := unify(ttIfx, ttElsex)
		       (ur2, fsubs) := unifyParameters(subs, elseSubs)
		       stdout << "Pre " << ur << " " << ur0 << newline
		       ur := mergeResults(ur, ur0)
		       stdout << "Step 1 " << ur << newline
		       ur := mergeResults(ur, ur2)
		       fsubs := apply(ur, fsubs)
		       stdout << "Final " << ur << " " << fsubs << newline
		       if not failed? ur then
		           tposs := cons((ur ttIfx, fsubs), tposs)
	    stdout << "if tposs " << [[sexpression tp, sexpression x] for (tp, x) in tposs]@SExpression << newline
	    annotate!(tbl, app, [cx for cx in tposs])
        inferExpression expr
	return tbl

    inferLambdaCounter: CallCounter := counter("inferLambda", false)
    inferLambda1(env: Env, infer: Expression -> (), tbl: Annotated(Expression, TPoss), e: Expression): TPoss ==
        matchParam(sym: Symbol, ps: ParamSet): TypeTerm ==
	    ssym := paramType(env, sym)
	    ptt := find(ssym, ps)
	    stdout << "match " << ps << ": " << ssym << " -> " << ptt << newline
	    not failed? ptt => forall(vars retract ptt, paramExpr expr retract ptt)
	    forall(newsym, expr newsym) where newsym := new()$Symbol
        lambdaVarType(decl: Expression, ps: ParamSet): TypeTerm ==
	    declare? decl => constant declareType decl
	    term? decl => matchParam(term decl, ps)
	    error "bad param"
        makeLambda(retType: TypeTerm, ps: ParamSet): TypeTerm ==
	    varTypes: List TypeTerm := [lambdaVarType(decl, ps) for decl in lambdaVars e]
	    allvars: ListSet Symbol := (+,[])/(vars tt for tt in varTypes)
	    mapExpr: Expression := apply(expr(-"map"),
	        [comma [expr var for var in varTypes], expr retType])
            forall(vars retType + allvars, mapExpr)
	count := open(inferLambdaCounter)
	writer(count) << "(lambda " << count << " " << e << newline
        infer(lambdaBody e)
	types := annotation(tbl, lambdaBody e)
	tp: TPoss := [(makeLambda(type, pset), pset) for (type, pset) in types]
	writer(count) << " lambda " << count << " " << e << " --> " << tp << ")" << newline
	close(count)
	return tp

    filterMaps(ops: TPoss): TPoss == [t for (t, l) in ops | map? t] -- FIXME: Loses subst

    filterOpsCounter: CallCounter := counter("filterOps", false) 
  
    filterOps(ops: TPoss, argPoss: List TPoss): TPoss ==
        count := open(filterOpsCounter)
        writer(count) << "Ops " << ops << newline
        writer(count) << "args " << argPoss << newline
	possR: List Cross(TypeTerm, ParamSet) := []
	for (opType, opSub) in ops repeat
	    possl: List Cross(UnificationResult, ParamSet) :=
	        [(result(empty(), []), opSub)@Cross(UnificationResult, ParamSet)]
	    for argNPoss in argPoss for i in 1.. repeat
	        opArgTf := mapArgN(opType, i)
	        stdout << "(Considering argument " << i << " = " << opArgTf << newline
	        newPossl: List Cross(UnificationResult, ParamSet) := []
	        for (argTf, subTf) in argNPoss for j in 1.. repeat
		    writer(count) << "Type " << i << "," << j << "/" << (#argNPoss) << " " << opArgTf << " <> " << argTf << newline
                    ur := unify(opArgTf, argTf)
		    writer(count) << "Type " << i << "," << j << "/" << (#argNPoss) << " = " << ur << newline
		    writer(count) << "PossL " << i << "," << j << "/" << (#argNPoss) << " = " << [sexpression(p) for (p, s) in possl] << newline
                    if not failed? ur then
		        for (poss, possSub) in possl repeat
			    urx := mergeResults(ur, poss)
			    (urx2, sub2) := unifyParameters(possSub, subTf)
			    urx := mergeResults(urx, urx2)
			    sub2 := apply(urx, sub2)
		            if not failed? urx then
			        newPossl := cons((urx, sub2), newPossl)
			    writer(count) << "Poss " << j << " " << urx << newline
	        possl := newPossl
		writer(count) << "Finished arg " << i << ")" << newline
	    writer(count) << "OpType " << opType << newline
	    writer(count) << "OpType - Results " << [[sexpression(l), sexpression(r)] for (l, r) in possl] << newline
	    possR := append!([(possN mapRet opType, sub) for (possN, sub) in possl], possR)
        finalTp: TPoss := [tt for tt in possR]
	writer(count) << "Types " << finalTp << newline
	close(filterOpsCounter)
        return finalTp
	
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

    local inferIf(expr: Expression, u: Unit Expression, tt: TypeTerm): () == never

    local inferTerm(expr: Expression, u: Unit Expression, tt: TypeTerm): () == never

Compiler: with
    compile: (Env, Expression) -> Unit Expression
== add
    import from BottomUp
    import from ScopeBind

    compile(env: Env, expr: Expression): Unit Expression ==
        unit: Unit Expression := new(expr)
	bind(unit, env)
	infer unit
	return unit
	
TestSubstitution: with
    testExpression: () -> ()
    testSubst: () -> ()
    testBottomUp: () -> ()
== add
    import from Compiler
    import from TypeTerm
    import from Symbol
    import from Annotated(Expression, TPoss)
    import from Assert Expression
    import from Assert SExpression
    import from Assert TPoss
    import from Partial Expression
    import from TPoss

    testExpression(): () ==
        import from Expression
        sx: SExpression := fromString "(|apply| |f| |x|)"
        e: Expression := fromString "(apply f x)"
	assertEquals(sexpression e, sx)

    testSubst(): () ==
        sigma: Substitution := sigma(-"x", fromString "(apply f a)")
	expr: Expression := fromString "(apply foo x)"
	assertEquals(fromString "(apply foo (apply f a))", sigma expr)

    testBottomUp(): () ==
        testBottomUpSimple()
	testBottomUpVar1()
	testBottomUpVar1Shared()
	testBottomUpVar2()
	testBottomUpVar3()
	testBottomUpIf()
	testBottomUpLambda()
	testBottomUpEmbeddedLambda0()
	testBottomUpEmbeddedLambda()
	testBottomUpLambdaIf1()
	testBottomUpLambdaIf2()
	testBottomUpLambdaIf3()
	testBottomUpLambdaIf4()
	
    testBottomUpSimple(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
        put!(env, -"+", fromString "(apply map (comma int int) int)")
	expr: Expression := fromString "n"

	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	tl := annotation(bup, expr)
	assertEquals([fromString "int"], tl)

	expr2: Expression := fromString "(apply + n n)"
	unit: Unit Expression := compile(env, expr2)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr2))

    testBottomUpVar1(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpVar1Shared(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"nil", fromString "(forall (t) (apply list t))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons nil nil)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	assertEquals([fromString "(forall (x) (apply list (apply list x)))"], annotation(bup, expr))

    testBottomUpVar2(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"n", fromString "float")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	assertEquals([fromString "(apply list float)", fromString "(apply list int)"], annotation(bup, expr))
	
    testBottomUpVar3(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"s", fromString "string")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
	put!(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
	put!(env, -"bracket", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	
	expr: Expression := fromString "(apply bracket (apply cons n nil) s)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	assertEquals([fromString "(apply pair (apply list int) string)"], annotation(bup, expr))

    testBottomUpIf(): () ==
        env: Env := empty()
	put!(env, -"n", fromString "int")
	put!(env, -"s", fromString "string")
	put!(env, -"=", fromString "(forall (t) (apply map (comma t t) bool))")
	put!(env, -"nil", fromString "(forall (x) (apply list x))")
    
	expr: Expression := fromString "(if (apply = n n) s s)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals([fromString "string"], annotation(bup, expr))
	assertEquals([fromString "bool"], annotation(bup, retract search(expr, fromString ("(apply = n n)"))))

	expr: Expression := fromString "(if (apply = n n) s n)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals(empty(), annotation(bup, expr))

    testBottomUpLambda(): () ==
        env: Env := empty()
	expr: Expression := fromString "(lambda ((: x int)) x)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

    testBottomUpEmbeddedLambda0(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) x) one)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr))

    testBottomUpEmbeddedLambda(): () ==
	env: Env := empty()
	put!(env, -"eval", fromString "(forall (a b) (apply map (comma (apply map (comma a) b) a) b))")
	put!(env, -"mklist", fromString "(forall (x) (apply map (comma x) (apply list x)))")
	put!(env, -"one", fromString "int")
        expr: Expression := fromString "(apply eval (lambda (x) (apply mklist x)) one)"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpLambdaIf1(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(forall (a) (apply map (comma a) bool))")
	put!(env, -"zero", fromString "int")
        expr: Expression := fromString "(lambda (x) (if (apply test x) x zero))"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))
    

    testBottomUpLambdaIf2(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(apply map (comma int) bool)")
	put!(env, -"minus", fromString "(forall (a) (apply map (comma a) a))")
	put!(env, -"one", fromString "int")
	put!(env, -"zero", fromString "int")
        expr: Expression := fromString "(lambda (x) (if (apply test x) x (apply minus x)))"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
        assertEquals([fromString "(apply map (comma int) int)"], annotation(bup, expr))

    testBottomUpLambdaIf3(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(apply map (comma int) bool)")
        expr: Expression := fromString "(lambda (x y) (if (apply test x) x y))"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
        assertEquals([fromString "(apply map (comma int int) int)"], annotation(bup, expr))

    testBottomUpLambdaIf4(): () ==
	env: Env := empty()
	put!(env, -"test", fromString "(forall (a) (apply map (comma a) bool))")
        expr: Expression := fromString "(lambda (x y) (if (apply test x) x y))"
	unit: Unit Expression := compile(env, expr)
	bup := bupAnnotations(unit)$BottomUp
	stdout << "infer " << bup << newline
        assertEquals([fromString "(forall (a) (apply map (comma a a) a))"], [t for (t, p) in annotation(bup, expr)])

-- case of multiple choices for parameters resulting in same type
-- (lambda (a) (foo a))
-- foo: (int) -> int
-- foo: (string) -> int
-- tposs on 'a'; Vx, (a=x)
-- tposs on (foo a a)
--    [int, (a=int)] [int, (a=string)]

#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestSubstitution
testExpression()
testSubst()
testBottomUp()
stdout << "DONE!" << newline
#endif
