#include "types"
#include "aldorio"
#pile

TypeTerm: SExpressionType with
    PrimitiveType
    vars: % -> ListSet Symbol
    expr: % -> Expression
    var?: (%, Symbol) -> Boolean
    forall: (Symbol, Expression) -> %
    forall: (ListSet Symbol, Expression) -> %
    constant: Expression -> %
    constant?: % -> Boolean

    part: (%, Expression -> Expression) -> %

    apply: ((Expression -> Expression), %) -> %
== add
    Rep == Record(vars: ListSet Symbol, expr: Expression)
    import from Rep, Symbol, String
    import from List ExpressionTree
    import from ListSet Symbol
    import from MachineInteger
    
    vars(t: %): ListSet Symbol == rep(t).vars
    expr(t: %): Expression == rep(t).expr

    var?(t: %, sym: Symbol): Boolean == member?(sym, vars t)

    part(t: %, f: Expression -> Expression): % == forall(vars t, f expr t)
    apply(f: Expression -> Expression, t: %): % == forall(vars t, f expr t)

    (a: %) = (b: %): Boolean == 
        tbl: HashTable(Symbol, Symbol) := []
        modEq(s1: Symbol, s2: Symbol): Boolean ==
	    var?(a, s1) ~= var?(b, s2) => false
	    not var?(a, s1) => s1 = s2
	    csym: Partial Symbol := find(s1, tbl)
	    if not failed? csym then tbl.s1 = s2
	    else 
	        tbl.s1 := s2
		true
        #vars(a) = #vars(b) and equalMod(modEq, expr(a), expr(b))

    constant?(e: %): Boolean == empty? vars e
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
	forall([sym sxi for sxi in vars], parseSExpression expr)

ParamSet: Join(SExpressionOutputType, PrimitiveType) with
    empty: () -> %
    param: (Symbol, TypeTerm) -> %
    param: TypeTerm -> %
    find: (Symbol, %) -> Partial TypeTerm
    generator: % -> Generator TypeTerm
    +: (%, %) -> %
    fv: % -> ListSet Symbol
    bracket: Generator TypeTerm -> %
    bracket: Tuple TypeTerm -> %

    empty?: % -> Boolean
== add
    Tbl == HashTable(Symbol, TypeTerm)
    Cx == Cross(Symbol, TypeTerm)
    Rep == Record(fv: ListSet Symbol, tbl: Tbl)
    import from Rep, Tbl, Expression
    import from TypeTerm, Symbol
    import from ListSet Symbol
    import from Fold2 ListSet Symbol
    import from MachineInteger

    fv(ps: %): ListSet Symbol == rep(ps).fv
    empty(): % == per [[], []]
    empty?(ps: %): Boolean == empty? rep(ps).tbl
    param(sym: Symbol, tt: TypeTerm): % == per [vars tt, [(sym, tt)@Cx]]
    param(p: TypeTerm): % == per [vars p, [ (paramVar expr p, forall(vars p, paramExpr expr p))@Cx]]
    find(sym: Symbol, ps: %): Partial TypeTerm ==
        return find(sym, rep(ps).tbl)

    generator(ps: %): Generator TypeTerm == v for (k, v) in rep(ps).tbl
    
    bracket(g: Generator TypeTerm): % ==
        l: List TypeTerm := [g]
        per [(union,[])/(vars tt for tt in l), [(paramVar expr tt, tt) for tt in l]]

    bracket(t: Tuple TypeTerm): % == [element(t, i) for i in 1..length t]
    
    (a: %) + (b: %): % ==
        tt: Tbl := [(k, v) for (k, v) in rep(a).tbl]
	for (k, v) in rep(b).tbl repeat
	    tt.k := v
        per [union(fv a, fv b), tt]

    (a: %) = (b: %): Boolean ==
        import from Partial TypeTerm
	import from BooleanFold
        fv a = fv b and _and/([p] = find(paramVar expr p, b) for p in a)

    sexpression(ps: %): SExpression ==
        [sexpr(-"pset"), [sexpr(v) for v in fv ps], [[sexpression term] for (k, term) in rep(ps).tbl]]

TestTypeTerm: with
    testEquality: () -> ()
    testParamSet: () -> ()
== add
    import from TypeTerm
    import from Assert TypeTerm
    import from Symbol
    import from Partial TypeTerm
    
    testEquality(): () ==
        assertEquals(fromString "x", fromString "x")
        assertEquals(fromString "(apply f x)", fromString "(apply f x)")
	assertEquals(fromString "(forall () x)", fromString "x")

	assertEquals(fromString "(forall (x) x)", fromString "(forall (x) x)")
	assertNotEquals(fromString "(forall (x) x)", fromString "(forall (x) k)")
	assertNotEquals(fromString "(forall (x) x)", fromString "x")

	assertEquals(fromString "(forall (x) x)", fromString "(forall (y) y)")
	equalityAxioms(fromString "(forall (x) (apply f x))", fromString "(forall (y) (apply f y))", fromString "(apply f x)")

	assertEquals(fromString "(forall (x y) (apply pair x y))", fromString "(forall (x y) (apply pair y x))")

    testParamSet(): () ==
        ps1: ParamSet := param(-"x", fromString "(forall (a) a)")
	assertTrue(not failed? find(-"x", ps1))
	
#if ALDORTEST
#include "types"
#pile
import from TestTypeTerm
testEquality()
testParamSet()
#endif
