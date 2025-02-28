#include "types"
#include "aldorio"
#pile

HType: SExpressionType with
    map?: % -> Boolean
    mapArgs: % -> List %
    mapRet: % -> %
    map: (List %, %) -> %

    term?: % -> Boolean
    termId: % -> Symbol
    term: Symbol -> %
== add
    MAP == List %
    TERM == Record(sym: Symbol)
    Rep == Union(map: MAP, term: TERM)
    import from Rep, MAP, TERM

    map?(tp: %): Boolean == rep(tp) case map
    mapArgs(tp: %): List % == rest rep(tp).map
    mapRet(tp: %): % == first rep(tp).map
    map(args: List %, ret: %): % == per [map == cons(ret, args)]

    term?(tp: %): Boolean == rep(tp) case term
    termId(tp: %): Symbol == rep(tp).term.sym
    term(sym: Symbol): % == per [term == [sym]]


    sexpression(tp: %): SExpression ==
        map? tp  => [sexpr(-"map"), [sexpression arg for arg in mapArgs tp], sexpression mapRet tp]
	term? tp => sexpr termId tp
	error "oops"

    parseSExpression(sx: SExpression): % == never

QType: SExpressionType with
    fv: % -> List Symbol
    type: % -> HType
    constant?: % -> Boolean
    constant: HType -> %

    map?: % -> Boolean
    mapArgs: % -> List %
    mapRet: % -> %
    map: (List %, %) -> %

    term?: % -> Boolean
    termId: % -> Symbol
    term: Symbol -> %
== add
    Rep == Record(fv: List Symbol, tp: HType)
    import from Rep
    import from List HType
    import from Symbol
    
    fv(qt: %): List Symbol == rep(qt).fv
    type(qt: %): HType == rep(qt).tp

    constant?(t: %): Boolean == empty? fv t
    constant(ht: HType): % == per [[], ht]
    forall(v: List Symbol, ht: HType): % == per [v, ht]

    map?(qt: %): Boolean == map? type qt
    mapArgs(tp: %): List % == [forall(fv tp, ht) for ht in mapArgs type tp]
    mapRet(tp: %): % == forall(fv tp, mapRet type tp)
    map(args: List %, ret: %): % == constant(map([type a for a in args], type ret))

    term?(qt: %): Boolean == term? type qt
    termId(qt: %): Symbol == termId type qt
    term(sym: Symbol): % == constant term sym

    sexpression(tp: %): SExpression ==
        constant? tp => sexpression type tp
	[sexpr(-"forall"), [sexpr(v) for v in fv tp], sexpression type tp]

    parseSExpression(sx: SExpression): % == never

QEnv: with
    empty: () -> %
    meaning: (%, Symbol) -> Partial QType
    addLevel: (%, List Cross(Symbol, QType)) -> %
== add
    Tbl == HashTable(Symbol, QType)
    Lvl == Record(tbl: Tbl)
    Rep == List Lvl
    import from Tbl, Rep, Lvl

    empty(): % ==
        tbl: Tbl := table()
        per [[tbl]]

    meaning(e: %, sym: Symbol): Partial QType ==
        empty? rep(e) => failed 
	maybe: Partial QType := find(sym, currentLevel(e).tbl)
	failed? maybe => meaning(parent e, sym)
	maybe

    local push(e: %): % == per cons([table()], rep e)

    addLevel(e: %, l: List Cross(Symbol, QType)): % ==
        newE := push(e)
	for (sym, tp) in l repeat
	    put!(newE, sym, tp)
	newE

    put!(e: %, sym: Symbol, qt: QType): () ==
        lvl := currentLevel(e)
        lvl.tbl.sym := qt

    local currentLevel(e: %): Lvl == first rep(e)
    local parent(e: %): % == per rest rep e

QConstraint: SExpressionType with
    eqConstraint: (QType, QType) -> %
    eqConstraint?: % -> Boolean
== add
    EQ == Record(lhs: QType, rhs: QType)
    Rep == Union(eq: EQ)
    import from Rep, EQ
    import from Symbol
    import from QType
    import from Integer
    
    eqConstraint?(c: %): Boolean == rep(c) case eq
    eqConstraint(lhs: QType, rhs: QType): % == per [eq == [lhs, rhs]]
    eqLhs(c: %): QType == rep(c).eq.lhs
    eqRhs(c: %): QType == rep(c).eq.rhs

    sexpression(tp: %): SExpression ==
        eqConstraint? tp =>
	    [sexpr(-"eqC"), sexpression eqLhs tp, sexpression eqRhs tp]
        never

    parseSExpression(sx: SExpression): % ==
        head := first sx
	not sym? head => error "not a constraint"
	sym head = -"eqC" =>
	    eqConstraint(parseSExpression nth(sx, 1), parseSExpression nth(sx, 2))
	error "unknown constraint"

HInfer: with
    constraints: (Unit Expression, QEnv) -> (Annotated(Expression, QType), List QConstraint)
    mark!: Unit Expression -> ()
    solve: (Unit Expression, List QConstraint) -> ()
== add
    import from HType
    import from Fold2 List QConstraint
    import from List Expression
    import from List Cross(QType, QConstraint)
    import from List QConstraint
    import from Partial QType
    import from QConstraint
    import from QType

    mark!(u: Unit Expression): () ==
        mark(e: Expression): () ==
	    term? e => markTerm e
	    apply? e => markApply e
	    lambda? e => markLambda e
	    let? e => markLet e
	    never
        markTerm(e: Expression): () == never
        markApply(e: Expression): () == never
        markLambda(e: Expression): () == never
        markLet(e: Expression): () == never
	mark(base u)
	
    newVar(): QType == term(new()$Symbol)

    constraints(u: Unit Expression, ctxt: QEnv): (Annotated(Expression, QType), List QConstraint) ==
        tbl: Annotated(Expression, QType) := new()
	(qt, cc) := constraints(ctxt, tbl, base u)
	return (tbl, cc)
	
    constraints(ctxt: QEnv, tbl: Annotated(Expression, QType), e: Expression): (QType, List QConstraint) ==
        constraints(ctxt: QEnv, e: Expression): (QType, List QConstraint) ==
	    term? e => constraintsTerm(ctxt, e)
	    apply? e => constraintsApply(ctxt, e)
	    lambda? e => constraintsLambda(ctxt, e)
	    let? e => constraintsLet(ctxt, e)
	    never
        constraintsLet(ctxt: QEnv, e: Expression): (QType, List QConstraint) ==
	    newVars: List Cross(Symbol, QType) := []
	    for ldef in letDefs e repeat
	        not def? ldef => error "expected a def"
	        not term? defLhs ldef => error "expected a var"
		lhsVar := term defLhs ldef
		newVars := cons( (lhsVar, newVar()), newVars)
	    newCtxt := addLevel(ctxt, newVars)
	    newConstraints: List QConstraint := []
	    for ldef in letDefs e for (sym, varType) in newVars repeat
	        (tp, varC) := constraints(newCtxt, defRhs ldef)
		topC := eqConstraint(varType, tp)
		newConstraints := cons(topC, append!(varC, newConstraints))
	    (bodyType, bodyC) := constraints(newCtxt, letBody e)
	    return (bodyType, append!(bodyC, newConstraints))
        constraintsTerm(ctxt: QEnv, e: Expression): (QType, List QConstraint) ==
	    tp := lookup(ctxt, term e) 
	    (tp, [])
	constraintsApply(ctxt: QEnv, e: Expression): (QType, List QConstraint) ==
	    (qt, cfn) := constraints(ctxt, applicationOp e)
	    argTypeAndConstraints: List Cross(QType, List QConstraint) := [constraints(ctxt, arg) for arg in applicationArgs e]
	    argTypes: List QType := [tt for (tt, qc) in argTypeAndConstraints]
	    argConstraints := (append!, [])/(qc for (tt, qc) in argTypeAndConstraints)
	    final: QType := newVar()
	    fnType := map(argTypes, final)
	    (final, cons(eqConstraint(qt, fnType), argConstraints))
	constraintsLambda(ctxt: QEnv, e: Expression): (QType, List QConstraint) ==
	    (bodyT, bodyC) := constraints(ctxt, lambdaBody e)
	    lambdaArgs: List QType := lambdaArgTypes(lambdaVars e)
	    final: QType := map(lambdaArgs, bodyT)
	    never
	constraints(ctxt, e)

    local lambdaArgTypes(e: List Expression): List QType == never

    solve(u: Unit Expression, c: List QConstraint): () == never

    lookup(e: QEnv, sym: Symbol): QType == retract meaning(e, sym)


TestHInfer: with
    test: () -> ()
== add
    import from HInfer
    import from Annotated(Expression, QType)
    import from List QConstraint
    import from List QType
    import from List Cross(Symbol, QType)
    import from QConstraint
    import from QType
    import from Symbol
    
    test(): () ==
        test0()

    test0(): () ==
        env: QEnv := empty()
	env := addLevel(env, [ (-"foo", map([term(-"int")], term(-"int"))),
	                       (-"one", term(-"int"))])
        e: Expression := fromString "(apply foo one)"
	u: Unit Expression := new e
	(tp, constrs) := constraints(u, env)
	stdout << "Type is " << "(" << newline
	for c in constrs repeat
	    stdout << "  " << c << newline
	stdout << ")" << newline
	
#if ALDORTEST
#include "types"
#include "aldorio"
#pile
test()$TestHInfer
stdout << "DONE!" << newline
#endif
