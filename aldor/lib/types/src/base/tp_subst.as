#include "types"
#include "aldorio"
#pile

Substitution: SExpressionType with
    PrimitiveType
    substs: % -> List Cross(Symbol, Expression)

    empty: () -> %
    addSubst: (%, Symbol, Expression) -> %
    #: % -> MachineInteger

    sigma: Generator Cross (Symbol, Expression) -> %
    sigma: Tuple Cross (Symbol, Expression) -> %
    sigma: (Symbol, Expression) -> %
    lookup: (sigma: %, sym: Symbol) -> Partial Expression
    var?: (Symbol, %) -> Boolean
    vars: % -> ListSet Symbol

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

    #(a: %): MachineInteger == # substs a
    substs(sigma: %): List Subst == rep(sigma)
    addSubst(sigma: %, sym: Symbol, expr: Expression): % == per cons( (sym, expr), rep(sigma))

    empty(): % == per []

    sigma(sym: Symbol, e: Expression): % == validate(per [ (sym, e)@Subst])
    sigma(pairs: Tuple Subst): % == per [pairs]

    generator(sigma: %): Generator Cross(Symbol, Expression) == (a,b) for (a, b) in rep sigma
#if 0    
        ll: List Subst := rep sigma
	len := #ll
	generate while not empty? ll repeat
	    (a, b) := first ll
	    ll := rest ll
	    yield (a, b)
#endif	    
    sigma(g: Generator Subst): % == validate(per [g])

    empty?(sigma: %):  Boolean == empty? rep sigma

    var?(sym: Symbol, sigma: %): Boolean == _or/((v = sym) for (v, e) in sigma)
    vars(sigma: %): ListSet Symbol == [v for (v, e) in sigma]

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
	comma? expr => comma [sigma p for p in commaParts expr]
	param? expr => param(paramVar expr, sigma paramExpr expr)
	stdout << "Expr " << expr << newline
        error "unknown expr in subst: "

    local subst(sigma: %, expr: Expression, cb: Symbol -> ()): Expression ==
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
        [[sexpr(-"subst"), sexpr sym, sexpression e] for (sym, e) in rep sigma]

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
