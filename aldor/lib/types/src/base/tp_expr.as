#include "types"
#include "aldorio"
#pile

-- FIXME: Alphabet!
Expression: SExpressionType with
    PrimitiveType
    term?: % -> Boolean
    term: % -> Symbol
    expr: Symbol -> %

    terms: % -> ListSet Symbol

    apply: (%, List %) -> %
    apply?: % -> Boolean -- FIXME..
    application?: % -> Boolean
    applicationOp: % -> %
    applicationArgs: % -> List %
    
    comma?: % -> Boolean
    comma: List % -> %
    commaParts: % -> List %
    emptyComma?: % -> Boolean
    comma: Tuple % -> %
    
    declare?: % -> Boolean
    declareVar: % -> Symbol
    declareType: % -> %
    declare: (Symbol, %) -> %

    if?: % -> Boolean
    _if: (%, %, %) -> %
    ifTest: % -> %
    ifPart: % -> %
    ifElsePart: % -> %

    lambda?: % -> Boolean
    lambda: (List %, Expression) -> %
    lambdaBody: % -> %
    lambdaVars: % -> List %

    param?: % -> Boolean
    paramVar: % -> Symbol
    paramExpr: % -> Expression
    param: (Symbol, %) -> %

    def?: % -> Boolean
    defLhs: % -> %
    defRhs: % -> %
    def: (%, %) -> %

    let?: % -> Boolean
    _let: (List %, %) -> %
    letDefs: % -> List Expression
    letBody: % -> Expression

    unknown: () -> %
    unknown?: % -> Boolean

    parts: % -> List %
    contains?: (%, Symbol) -> Boolean

    equalMod: ((Symbol, Symbol) -> Boolean, %, %) -> Boolean

    search: (%, %) -> Partial %
    all: (%, %) -> List %
== add
    App == Record(op: %, args: List %)
    Comma == List %
    Lambda == Record(vars: List %, body: %)
    If == Record(test: %, ifPart: %, elsePart: %)
    Declare == Record(var: Symbol, type: %)
    Param  == Record(sym: Symbol, rhs: %)
    Def == Record(lhs: %, rhs: %)
    Let  == Record(defs: List %, body: %)
    Unk  == Cross()
    Rep == Union(term: Symbol, app: App, comma: Comma, lambda: Lambda, _if: If, declare: Declare, pp: Param, _let: Let, def: Def, u: Unk)

    import from Rep, App, Symbol, Param, Def
    import from Fold2 List %
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
    comma(tExpr: Tuple %): % == per [[element(tExpr, n) for n in 1..length tExpr]]
    commaParts(e: %): List % == rep(e).comma
    emptyComma?(e: %): Boolean == comma? e and empty? commaParts e

    lambda?(e: %): Boolean == rep(e) case lambda
    lambda(l: List %, body: %): % == per [lambda==[l, body]]
    lambdaBody(e: %): % == rep(e).lambda.body
    lambdaVars(e: %): List % == rep(e).lambda.vars

    if?(e: %): Boolean == rep(e) case _if
    _if(t: %, ifPart: %, elsePart: %): % == per [_if==[t, ifPart, elsePart]]
    ifTest(e: %): % == rep(e)._if.test
    ifPart(e: %): % == rep(e)._if.ifPart
    ifElsePart(e: %): % == rep(e)._if.elsePart

    declare?(e: %): Boolean == rep(e) case declare
    declareVar(e: %): Symbol == rep(e).declare.var
    declareType(e: %): % == rep(e).declare.type
    declare(v: Symbol, e: %): % == per [declare==[v, e]]

    param?(e: %): Boolean == rep(e) case pp
    paramVar(e: %): Symbol == rep(e).pp.sym
    paramExpr(e: %): Expression == rep(e).pp.rhs
    param(v: Symbol, e: %): % == per [pp==[v, e]]

    def?(e: %): Boolean == rep(e) case def
    defLhs(e: %): % == rep(e).def.lhs
    defRhs(e: %): % == rep(e).def.rhs
    def(lhs: %, rhs: %): % == per [def==[lhs, rhs]]

    let?(e: %): Boolean == rep(e) case _let
    _let(defs: List %, body: %): % == per [_let == [defs, body]]
    letDefs(e: %): List % == rep(e)._let.defs
    letBody(e: %): % == rep(e)._let.body

    unknown(): % == per[u==()]
    unknown?(e: %): Boolean == rep(e) case u
    
    parts(e: %): List % ==
        term? e => []
	apply? e => cons(applicationOp e, applicationArgs e)
	comma? e => commaParts e
	lambda? e => [lambdaBody e]
	if? e => [ifTest e, ifPart e, ifElsePart e]
	declare? e => [declareType e]
	param? e => [paramExpr e]
	let? e => cons(letBody e, letDefs e)
        error "Missing case in parts"
	
    (expr1: %) = (expr2: %): Boolean ==
        term? expr1 =>
            term? expr2 and term expr1 = term expr2
        application? expr1 => application? expr2 and parts expr1 = parts expr2
        comma? expr1 => comma? expr2 and parts expr1 = parts expr2
	declare? expr1 => declare? expr2 and declareVar expr1 = declareVar expr2 and parts expr1 = parts expr2
        if? expr1 => if? expr2 and parts expr1 = parts expr2
	let? expr1 => let? expr2 and parts expr1 = parts expr2
	param? expr1 => param? expr2 and paramVar expr1 = paramVar expr2 and parts expr1 = parts expr2
        error "Missing case in equal"

    equalMod(symEq: (Symbol, Symbol) -> Boolean, expr1: %, expr2: %): Boolean ==
        term? expr1 =>
            term? expr2 and symEq(term expr1, term expr2)
        application? expr1 => application? expr2 and _and/(equalMod(symEq, p1, p2) for p1 in parts expr1 for p2 in parts expr2)
        comma? expr1 => comma? expr2 and _and/(equalMod(symEq, p1, p2) for p1 in parts expr1 for p2 in parts expr2)
        if? expr1 => if? expr2 and _and/(equalMod(symEq, p1, p2) for p1 in parts expr1 for p2 in parts expr2)
	declare? expr1 => declare? expr2 and equalMod(symEq, declareType(expr1), declareType(expr2))
        let? expr1 => let? expr2 and _and/(equalMod(symEq, p1, p2) for p1 in parts expr1 for p2 in parts expr2)
	param? expr1 => param? expr2 and paramVar expr1 = paramVar expr2 and equalMod(symEq, paramExpr(expr1), paramExpr(expr2))
	unknown? expr1 => unknown? expr2
        error "Missing case in equalMod"

    contains?(e: %, v: Symbol): Boolean ==
        term? e => term e = v
	_or/(contains?(arg, v) for arg in parts e)

    terms(expr: %): ListSet Symbol ==
        termsl(l: List %): ListSet Symbol ==
	    empty? l => []
	    terms first l + termsl rest l
        term? expr => [term expr]
        application? expr => terms applicationOp expr + termsl applicationArgs expr
	termsl parts expr

    search(e: %, tgt: %): Partial % ==
        e = tgt => [e]
	for p in parts e repeat
	   maybe := search(p, tgt)
	   if not failed? maybe then return maybe
	failed

    all(e: %, tgt: %): List % ==
        e = tgt => [e]
	(append!,[])/(all(p, tgt) for p in parts e)

    sexpression(e: %): SExpression ==
        import from MachineInteger
        term? e => sexpr term e
	apply? e => cons(sexpr(-"apply"), cons(sexpression applicationOp e, [sexpression arg for arg in applicationArgs e]))
	comma? e => cons(sexpr(-"comma"), [sexpression p for p in commaParts e])
	lambda? e => [sexpr(-"lambda"), [sexpression v for v in lambdaVars e], sexpression lambdaBody(e)]
	if? e => [sexpr(-"if"), sexpression ifTest e, sexpression ifPart e, sexpression ifElsePart e]
	declare? e => [sexpr(-":"), sexpr declareVar e, sexpression declareType e]
	let? e => [sexpr(-"let"), [sexpression v for v in letDefs e], sexpression letBody e]
	param? e => [sexpr(-"param"), sexpr paramVar e, sexpression paramExpr e]
	unknown? e => sexpr(-"*unknown*")
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
	    lambda([parseSExpression sxi for sxi in first sx], parseSExpression first rest sx)
	s0 = -"let" =>
	    sx := rest sx
	    _let([parseSExpression sxi for sxi in first sx], parseSExpression first rest sx)
	s0 = -"if" =>
	    sx := rest sx
	    testPart := parseSExpression first sx
	    sx := rest sx
	    ifPart := parseSExpression first sx
	    sx := rest sx
	    elsePart := parseSExpression first sx
	    _if(testPart, ifPart, elsePart)
	s0 = -"declare" or s0 = -":" =>
	    sx := rest sx
	    declare(sym first sx, parseSExpression first rest sx)
	s0 = -"param" =>
	    sx := rest sx
	    param(sym first sx, parseSExpression first rest sx)
	stdout << "unknown " << sx << newline
	never    
