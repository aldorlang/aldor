#include "algebra"
#include "aldorio"
#pile

ListSet(T: PrimitiveType): PrimitiveType with
    if T has ExpressionType then ExpressionType
    if T has SExpressionOutputType then SExpressionOutputType
    generator: % -> Generator T
    cons: (t: T, s: %) -> %
    remove: (s: %, t: T) -> %
    bracket: Tuple T -> %
    bracket: Generator T -> %
    member?: (T, %) -> Boolean
    +: (%, %) -> %
    -: (%, %) -> %
    intersect: (%, %) -> %
    empty: () -> %
    empty?: % -> Boolean
    #: % -> MachineInteger
== List T add
    Rep == List T
    import from Rep
    import from BooleanFold
    import from T

    empty(): % == per []
    cons(t: T, s: %): % == if member?(t, s) then s else per cons(t, rep s)
    member?(t: T, s: %): Boolean == _or/(elt = t for elt in rep s)
    remove(s: %, t: T): % == per [ x for x in s | x ~= t]

    empty?(s: %): Boolean == empty? rep s

    (a: %) + (b: %): % == per append!(copy rep a, [elt for elt in b | not member?(elt, a)])
    (a: %) - (b: %): % == per [elt for elt in rep a | not member?(elt, rep b)]

    if T has ExpressionType then
        import from List ExpressionTree
        extree(s: %): ExpressionTree == ExpressionTreeList [extree e for e in rep s]

    (a: %) = (b: %): Boolean ==
        import from MachineInteger
        empty? a => empty? b
        # a = # b and _and/(member?(x, rep a) for x in rep b)

    if T has SExpressionOutputType then
        sexpression(l: %): SExpression == [sexpression t for t in l]

Expression: SExpressionType with
    PrimitiveType
    term?: % -> Boolean
    term: % -> Symbol
    expr: Symbol -> %

    apply?: % -> Boolean
    
    terms: % -> ListSet Symbol

    application?: % -> Boolean
    applicationOp: % -> %
    applicationArgs: % -> List %
    apply: (%, List %) -> %

    comma?: % -> Boolean
    comma: List % -> %
    commaParts: % -> List %

    if?: % -> Boolean
    _if: (%, %, %) -> %

    lambda?: % -> Boolean
    lambda: (List Symbol, Expression) -> %

    parts: % -> List %
    contains?: (%, Symbol) -> Boolean
== add
    App == Record(op: %, args: List %)
    Comma == List %
    Lambda == Record(vars: List Symbol, body: %)
    If == Record(test: %, ifPart: %, elsePart: %)
    Rep == Union(term: Symbol, app: App, comma: Comma, lambda: Lambda, _if: If)
    import from Rep, App, Symbol
    import from List %
    import from BooleanFold
    import from MachineInteger
    
    term?(e: %): Boolean == rep e case term
    term(e: %): Symbol == rep(e).term
    expr(s: Symbol): % == per [s]

    application?(e: %): Boolean == rep(e) case app
    applicationOp(e: %): % == rep(e).app.op
    applicationArgs(e: %): List % == rep(e).app.args

    apply?(e: %): Boolean == rep(e) case app
    apply(op: %, args: List %): % == per [[op, args]]

    comma?(e: %): Boolean == rep(e) case comma
    comma(l: List %): % == per [l]
    commaParts(e: %): List % == rep(e).comma

    lambda?(e: %): Boolean == rep(e) case lambda
    lambda(l: List Symbol, body: %): % == per [[l, body]]
    lambdaBody(e: %): % == rep(e).lambda.body
    lambdaVars(e: %): List Symbol == rep(e).lambda.vars

    if?(e: %): Boolean == rep(e) case _if
    _if(t: %, ifPart: %, elsePart: %): % == per [[t, ifPart, elsePart]]

    parts(e: %): List % ==
        term? e => []
	apply? e => cons(applicationOp e, applicationArgs e)
	comma? e => commaParts e
	lambda? e => [lambdaBody e]
	if? e => [ifTest e, ifPart e, ifElsePart e]
	never
	
    (expr1: %) = (expr2: %): Boolean ==
        term? expr1 =>
            term? expr2 and term expr1 = term expr2
        application? expr1 => application? expr2 and parts expr1 = parts expr2
        comma? expr1 => comma? expr2 and parts expr1 = parts expr2
        never

    contains?(e: %, v: Symbol): Boolean ==
        term? e => term e = v
        application? e => contains?(applicationOp e, v) or _or/(contains?(arg, v) for arg in applicationArgs e)
        comma? e => _or/(contains?(arg, v) for arg in commaParts e)
        never

    terms(expr: %): ListSet Symbol ==
        termsl(l: List %): ListSet Symbol ==
	    empty? l => []
	    terms first l + termsl rest l
        term? expr => [term expr]
        application? expr => terms applicationOp expr + termsl applicationArgs expr
	comma? expr => termsl commaParts expr
        never

    sexpression(e: %): SExpression ==
        term? e => sexpr term e
	apply? e => cons(sexpr(-"apply"), cons(sexpression applicationOp e, [sexpression arg for arg in applicationArgs e]))
	comma? e => cons(sexpr(-"comma"), [sexpression p for p in commaParts e])
	lambda? e => [sexpr(-"lambda"), [sexpr v for v in lambdaVars e], lambdaBody(e)]
	stdout << "unexpected"
	never

    parseSExpression(sx: SExpression): % ==
        sym? sx => expr sym sx
	not sym? first sx => error("Bad form - " + toString sx)
	s0 := sym first sx
	s0 = -"apply" =>
	    sx := rest sx
	    apply(parseSExpression first sx, [parseSExpression sxi for sxi in rest sx])
	s0 = -"comma" =>
	    comma [parseSExpression sxi for sxi in rest sx]
	s0 = -"lambda" =>
	    sx := rest sx
	    lambda(sym sxi for sxi in first sx, parseSExpression first rest sx)
	stdout << "unknown " << sx << newline
	never    

TypeTerm: SExpressionType with
    PrimitiveType
    vars: % -> ListSet Symbol
    expr: % -> Expression
    var?: (%, Symbol) -> Boolean
    forall: (Symbol, Expression) -> %
    forall: (ListSet Symbol, Expression) -> %
    constant: Expression -> %
== add
    Rep == Record(vars: ListSet Symbol, expr: Expression)
    import from Rep, Symbol, String
    import from List ExpressionTree
    import from ListSet Symbol
    vars(t: %): ListSet Symbol == rep(t).vars
    expr(t: %): Expression == rep(t).expr

    var?(t: %, sym: Symbol): Boolean == member?(sym, vars t)

    (a: %) = (b: %): Boolean == vars(a) = vars(b) and expr(a) = expr(b)

    constant(e: Expression): % == per [empty(), e]
    forall(sym: Symbol, e: Expression): % == forall([sym], e)
    forall(vars: ListSet Symbol, e: Expression): % == per [vars, e]

    sexpression(tt: %): SExpression ==
        [sexpr(-"forall"), [sexpr s for s in vars tt], sexpression expr tt]

    parseSExpression(sx: SExpression): % ==
	not cons? sx => constant(parseSExpression sx)
	s0 := first sx
	s0 ~= sexpr(-"forall") =>
	    constant parseSExpression sx
	vars := first rest sx
	expr := first rest rest sx
	stdout << "Forall " << vars << " " << expr << newline
	forall([sym sxi for sxi in vars], parseSExpression expr)
	
Substitution: SExpressionType with
    PrimitiveType
    substs: % -> List Cross(Symbol, Expression)

    empty: () -> %
    addSubst: (%, Symbol, Expression) -> %

    sigma: Generator Cross (Symbol, Expression) -> %
    sigma: Tuple Cross (Symbol, Expression) -> %
    sigma: (Symbol, Expression) -> %
    lookup: (sigma: %, sym: Symbol) -> Partial Expression
    var?: (Symbol, %) -> Boolean

    empty?: % -> Boolean
    split: % -> (Symbol, Expression, %)

    compose: (%, %) -> %

    generator: % -> Generator Cross(Symbol, Expression)

    apply: (%, e: Expression) -> Expression

    validate: % -> %
== add
    Subst == Cross(Symbol, Expression)
    Rep  == List Subst
    import from Rep, Expression
    import from List ExpressionTree
    import from ExpressionTreeLeaf
    import from BooleanFold
    import from MachineInteger
    import from Symbol
    import from ExpressionTree
    
    substs(sigma: %): List Subst == rep(sigma)
    addSubst(sigma: %, sym: Symbol, expr: Expression): % == per cons( (sym, expr), rep(sigma))

    empty(): % == per []

    sigma(sym: Symbol, e: Expression): % == validate(per [ (sym, e)@Subst])
    sigma(pairs: Tuple Subst): % == validate(per [pairs])

    generator(sigma: %): Generator Subst == generator rep sigma
    sigma(g: Generator Subst): % == validate(per [g])

    empty?(sigma: %):  Boolean == empty? rep sigma

    var?(sym: Symbol, sigma: %): Boolean == _or/((v = sym) for (v, e) in sigma)

    split(sigma: %): (Symbol, Expression, %) ==
        (sym, expr) := first rep sigma
        (sym, expr, per rest rep sigma)


    lookup(sigma: %, sym: Symbol): Partial Expression ==
        for (sym2, expr) in rep sigma repeat
            if sym2 = sym then return [expr]
        failed

    compose(s1: %, s2: %): % ==
        -- result of s1 then s2
	l1: Rep := [(v, s2.e) for (v, e) in s1]
	l2: Rep := [(v, e) for (v, e) in s2 | not var?(v, s1)]
	--for (v,e) in l1 repeat stdout << "L1 " << v << " -> " << e << newline
	--for (v,e) in l2 repeat stdout << "L2 " << v << " -> " << e << newline
	sigma( (v, e) for (v, e) in append!(l1, l2))

    apply(sigma: %, expr: Expression): Expression ==
        import from Partial Expression, List Expression
        application? expr => apply(sigma applicationOp expr, [sigma e for e in applicationArgs expr])
        term? expr =>
            pp: Partial Expression := lookup(sigma, term expr)
            if failed? pp then expr else retract pp
        never

    subst(sigma: %, expr: Expression, cb: Symbol -> ()): Expression ==
        import from Partial Expression, List Expression
        application? expr => apply(sigma applicationOp expr, [sigma e for e in applicationArgs expr])
        term? expr =>
            pp: Partial Expression := lookup(sigma, term expr)
            failed? pp => expr
	    cb term expr
	    retract pp
        never

    (a: %) = (b: %): Boolean ==
        chk(v: Symbol, e: Expression): Boolean ==
	    r: Partial Expression := lookup(b, v)
	    not failed? r and retract r = e
        # substs a = # substs b and _and/(chk(v, e) for (v, e) in substs a)

    sexpression(sigma: %): SExpression ==
        [[sexpr(-"subst"), sexpr sym, sexpression e] for (sym, e) in sigma]

    parseSExpression(sx: SExpression): % == never

    validate(sigma: %): % == sigma
#if 0    
	l: List String := []
	varsAsSet: Set Symbol := [v for (v, e) in substs sigma]
        if # substs sigma ~= # varsAsSet then l := cons("duplicate syms", l)
	for var in varsAsSet repeat
	    for (v, e) in substs sigma repeat
	    	if contains?(e, var) then l := cons("contain check " + (toString var) + " " + toString(e), l)
        not empty? l => error("subst " + (toString extree sigma) + " --> " + toString l)
	return sigma
#endif

TypeTermSubst: SExpressionOutputType with
== add
    Rep == Record(sigma: Substitution, fv: HashTable(Symbol, TypeTerm))
    import from Rep
    
    bracket(g: Generator Cross(Symbol, TypeTerm)): % == never
	
    apply(sigma: %, tt: TypeTerm): TypeTerm == never

    sexpression(sub: %): SExpression == never
#if 0
   Vx, Foo X
   subst X -> Va, a
   --> Va, Foo a
   

#endif

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
	
TypeOperatorsCategory: Category == with
    unify2: (TypeTerm, TypeTerm) -> (UnificationResult, UnificationResult)
    unify: (TypeTerm, TypeTerm) -> UnificationResult
    apply: (UnificationResult, TypeTerm) -> TypeTerm
    mergeResults: (UnificationResult, UnificationResult) -> UnificationResult

    default
        unify2(tt1: TypeTerm, tt2: TypeTerm): (UnificationResult, UnificationResult) ==
            r := unify(tt1, tt2)
            (r, r)


Operators: TypeOperatorsCategory with
== add
    import from Expression, Substitution
    import from ListSet Symbol, Symbol
    import from TypeTerm
    import from MachineInteger
    import from List Expression
    
    unify(tf1: TypeTerm, tf2: TypeTerm): UnificationResult ==
        stdout << "(Unify " << tf1 << " <--> " << tf2 << newline
	sv := intersect(vars tf1, vars tf2)
	if not empty? sv then
	    unifyShared(tf1, tf2, sv)
        result := unify1(tf1, tf2)
        stdout << " Unify " << tf1 << " <--> " << tf2 << " ===> " << result << ")" << newline
        validate(result)
        result

    local unifyShared(tf1: TypeTerm, tf2: TypeTerm, shared: ListSet Symbol): UnificationResult
        replace: List Cross(Symbol, Symbol) := [(v, new()) for v in shared]
        sigmaR := sigma( (old, expr new) for (old, new) in replace)
	newTf2 := apply(result(newVars, sigmaR), tf2)
	ur := unify(tf1, newTf2)
	return ur
	
    local unify1(tf1: TypeTerm, tf2: TypeTerm): UnificationResult ==
        term? expr tf2 and not term? expr tf1 => unify(tf2, tf1)
        term? expr tf1 and term? expr tf2 =>
            var?(tf1, term expr tf1) and var?(tf2, term expr tf2) =>
                newVar: Symbol := new()
                result( sigma( (term expr tf1, expr newVar), (term expr tf2, expr newVar)),
                                [newVar])
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

    mergeResults(r1: UnificationResult, r2: UnificationResult): UnificationResult ==
        stdout << "(Merge " << r1 << " + " << r2 << newline
        res := mergeResults1(r1, r2)
        stdout << " Merge " << r1 << " + " << r2 << " ==> " << res << ")" << newline
        valid: List String := validate(res)
        if not empty? valid then
            stdout << "Invalid Result " << valid << newline
            never
        res

    local mergeResults1(r1: UnificationResult, r2: UnificationResult): UnificationResult ==
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
            stdout << " Merge Step " << var << " -> " << expr << " ==> " << final << newline
        validate final
        final

    apply(result: UnificationResult, tt: TypeTerm): TypeTerm ==
        import from ListSet Symbol, List Cross(Symbol, Expression)
        newExpr := (sigma result) expr tt
        forall(fv result + vars tt - [sym for (sym, expr) in sigma result], newExpr)

Annotated(Base: with, A: with): with
    new: () -> %
    annotation: (%, Base) -> A
    annotate!: (%, Base, A) -> ()
    if Base has OutputType and A has OutputType then OutputType
    
== add
    Rep == HashTable(Pointer, A)
    import from Rep, Base, A

    new(): % == per []
    annotation(tbl: %, o: Base): A == rep(tbl).(o pretend Pointer)
    annotate!(tbl: %, o: Base, a: A) ==
        rep(tbl).(o pretend Pointer) := a

    if Base has OutputType and A has OutputType then
      (w: TextWriter) << (tbl: %): TextWriter ==
        w << "("
	for (k, v) in rep tbl repeat
	    w << "(" << (k pretend Base) << " " << v << ") "
	w << ")"

Env: with
    empty: () -> %
    meanings: (%, Symbol) -> List TypeTerm
    put: (%, Symbol, TypeTerm) -> ()
== add
    Rep == HashTable(Symbol, List TypeTerm)
    import from Rep, List TypeTerm

    empty(): % == per forget(defVal)

    meanings(e: %, sym: Symbol): List TypeTerm == rep(e).sym
    put(e: %, sym: Symbol, t: TypeTerm): () ==
        rep(e).sym := cons(t, rep(e).sym)

    local defVal(k: Symbol): List TypeTerm == []

BottomUp: with
    infer: (Env, Expression) -> Annotated(Expression, List TypeTerm)
== add
    import from List Expression, List TypeTerm
    import from List List TypeTerm
    import from Expression
    import from Substitution
    import from Symbol
    import from Operators
    import from MachineInteger
    import from ListSet Symbol
    import from TypeTerm
    import from UnificationResult
    
    infer(e: Env, expr: Expression): Annotated(Expression, List TypeTerm) ==
        tbl: Annotated(Expression, List TypeTerm) := new()
        inferExpression(e: Expression): () ==
	    stdout << "(infer " << e << newline
	    inferExpression1 e
	    stdout << " infer " << e << " --> " << annotation(tbl, e) << ")" << newline
        inferExpression1(e: Expression): () ==
	    term? e => inferTerm e
	    apply? e => inferApply e
	    never
        inferTerm(termExpr: Expression): () ==
	    annotate!(tbl, termExpr, meanings(e, term termExpr))
	inferApply(app: Expression): () ==
	    inferExpression(applicationOp app)
	    for arg in applicationArgs app repeat
	        inferExpression(arg)
            tOp: List TypeTerm := annotation(tbl, applicationOp app)
            maps := filterMaps(tOp)
	    annotate!(tbl, app, filterOps(tOp, [annotation(tbl, arg) for arg in applicationArgs app]))
        inferExpression expr
	return tbl

    filterMaps(ops: List TypeTerm): List TypeTerm == [t for t in ops | map? t]
  
    filterOps(ops: List TypeTerm, argPoss: List List TypeTerm): List TypeTerm ==
	possR: List TypeTerm := []
	for opType in ops repeat
	    possl: List UnificationResult := [result(empty(), [])]
	    for argNPoss in argPoss for i in 1.. repeat
	        opArgTf := mapArgN(opType, i)
	        stdout << "Considering argument " << i << " = " << opArgTf << newline
	        newPossl: List UnificationResult := []
	        for argTf in argNPoss for j in 1.. repeat
		    stdout << "Type " << j << "/" << (#argNPoss) << " " << argTf << newline
                    ur := unify(opArgTf, argTf)
		    stdout << "Type " << j << "/" << (#argNPoss) << " = " << ur << newline
                    if not failed? ur then
		        for poss in possl repeat
			    ur := mergeResults(ur, poss)
		            newPossl := cons(ur, newPossl)
	        possl := newPossl
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

TestSubstitution: with
    testExpression: () -> ()
    testSubst: () -> ()
    testUnify: () -> ()
    testBottomUp: () -> ()
== add
    Rep == TypeOperatorsCategory

    import from Symbol, String
    import from Operators
    import from ListSet Symbol
    import from Assert TypeTerm
    import from Assert List TypeTerm
    import from Assert Expression
    import from Assert SExpression
    import from Assert Substitution
    import from Env
    import from TypeTerm
    import from BottomUp
    import from Annotated(Expression, List TypeTerm)
    import from List TypeTerm
    
    new(ops: TypeOperatorsCategory): % == per ops

    local test(tgt: %, tt1: TypeTerm, tt2: TypeTerm): () ==
        r: UnificationResult := unify(tt1, tt2)$(rep tgt)
        assertEquals( r tt1, r tt2)

    local test2(tgt: %, tt1: TypeTerm, tt2: TypeTerm): () ==
        (r1, r2) := unify2(tt1, tt2)$(rep tgt)
        assertEquals( r1 tt1, r2 tt2)

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
	test(new Operators, e1, e2)

    testUnifyApply1(): () ==
        e1: TypeTerm := fromString "(forall (x) (apply foo x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(new Operators, e1, e2)

    testUnifyApply2(): () ==
        e1: TypeTerm := fromString "(forall (f) (apply f x))"
	e2: TypeTerm := fromString "(forall (k) (apply foo k))"
	test(new Operators, e1, e2)

    testBottomUp(): () ==
        testBottomUpSimple()
	testBottomUpVar1()
	testBottomUpVar2()
	testBottomUpVar3()

    testBottomUpSimple(): () ==
        env: Env := empty()
	put(env, -"n", fromString "int")
        put(env, -"+", fromString "(apply map (comma int int) int)")
	expr: Expression := fromString "n"
	bup := infer(env, expr)
	tl := annotation(bup, expr)
	assertEquals([fromString "int"], tl)

	expr2: Expression := fromString "(apply + n n)"
	bup := infer(env, expr2)
	stdout << "infer " << bup << newline
	assertEquals([fromString "int"], annotation(bup, expr2))

    testBottomUpVar1(): () ==
        env: Env := empty()
	put(env, -"n", fromString "int")
	put(env, -"nil", fromString "(forall (x) (apply list x))")
	put(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	bup := infer(env, expr)
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpVar1(): () ==
        env: Env := empty()
	put(env, -"n", fromString "int")
	put(env, -"nil", fromString "(forall (t) (apply list t))")
	put(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	bup := infer(env, expr)
	assertEquals([fromString "(apply list int)"], annotation(bup, expr))

    testBottomUpVar2(): () ==
        env: Env := empty()
	put(env, -"n", fromString "int")
	put(env, -"n", fromString "float")
	put(env, -"nil", fromString "(forall (x) (apply list x))")
	put(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")

        expr: Expression := fromString "(apply cons n nil)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply list float)", fromString "(apply list int)"], annotation(bup, expr))
	
    testBottomUpVar3(): () ==
        env: Env := empty()
	put(env, -"n", fromString "int")
	put(env, -"s", fromString "string")
	put(env, -"nil", fromString "(forall (x) (apply list x))")
	put(env, -"cons", fromString "(forall (t) (apply map (comma t (apply list t)) (apply list t)))")
	put(env, -"bracket", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	
	expr: Expression := fromString "(apply bracket (apply cons n nil) s)"
	bup := infer(env, expr)
	stdout << "infer " << bup << newline
	assertEquals([fromString "(apply pair (apply list int) string)"], annotation(bup, expr))
	
#if ALDORTEST
#include "algebra"
#pile
import from TestSubstitution
testExpression()
testSubst()
testUnify()
testBottomUp()
#endif
