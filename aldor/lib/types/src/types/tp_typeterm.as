#include "types"
#include "aldorio"
#pile

TypeTerm: SExpressionType with
    PrimitiveType
    vars: % -> ListSet Symbol -- TODO: vars --> fv
    expr: % -> Expression
    var?: (%, Symbol) -> Boolean
    forall: (Symbol, Expression) -> %
    forall: (ListSet Symbol, Expression) -> %
    constant: Expression -> %
    constant?: % -> Boolean

    part: (%, Expression -> Expression) -> %

    unknown: () -> %
    unknown?: % -> Boolean
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
        --#vars(a) = #vars(b) and
	equalMod(modEq, expr(a), expr(b))

    constant?(e: %): Boolean == empty? vars e
    constant(e: Expression): % == per [empty(), e]
    forall(sym: Symbol, e: Expression): % == forall([sym], e)
    forall(vars: ListSet Symbol, e: Expression): % == per [vars, e]

    unknown(): % == constant(unknown()$Expression)
    unknown?(tt: %): Boolean == unknown? expr tt

    sexpression(tt: %): SExpression ==
        [sexpr(-"forall"), [sexpr s for s in vars tt], sexpression expr tt]

    parseSExpression(sx: SExpression): % ==
	not cons? sx => constant(parseSExpression sx)
	s0 := first sx
	s0 ~= sexpr(-"forall") =>
	    constant parseSExpression sx
	not list? first rest sx => error "Not a list"
	vars := first rest sx
	expr := first rest rest sx
	forall([sym sxi for sxi in vars], parseSExpression expr)

ParamSet: Join(SExpressionType, PrimitiveType) with
    empty: () -> %
    param: (Symbol, TypeTerm) -> %
    param: TypeTerm -> %
    find: (Symbol, %) -> Partial TypeTerm
    generator: % -> Generator TypeTerm
    +: (%, %) -> %
    vars: % -> ListSet Symbol

    bracket: Generator TypeTerm -> %
    bracket: Tuple TypeTerm -> %
    fv: % -> ListSet Symbol

    empty?: % -> Boolean
== add
    Tbl == HashTable(Symbol, Expression)
    Cx == Cross(Symbol, Expression)
    Rep == Record(fv: ListSet Symbol, tbl: Tbl)
    import from Rep, Tbl, Expression
    import from TypeTerm, Symbol
    import from ListSet Symbol
    import from Fold2 ListSet Symbol
    import from MachineInteger
    import from Partial Expression
    import from Partial TypeTerm

    fv(ps: %): ListSet Symbol == rep(ps).fv
    empty(): % == per [[], []]
    empty?(ps: %): Boolean == empty? rep(ps).tbl
    param(sym: Symbol, tt: TypeTerm): % ==
        param? expr tt => error "ooops"
        per [vars tt, [(sym, param(sym, expr tt))@Cx]]
    
    param(p: TypeTerm): % == per [vars p, [ (paramVar expr p, expr p)@Cx]]
    find(sym: Symbol, ps: %): Partial TypeTerm ==
        v := find(sym, rep(ps).tbl)
	failed? v => failed
	[forall(vars ps, retract v)]

    vars(ps: %): ListSet Symbol == [k for (k, v) in rep(ps).tbl]

    generator(ps: %): Generator TypeTerm == forall(fv ps, v) for (k, v) in rep(ps).tbl
    
    bracket(g: Generator TypeTerm): % ==
        l: List TypeTerm := [g]
	for tt in l repeat
	    if not param? expr tt then error "not a param term"
        per [(union,[])/(vars tt for tt in l), [(paramVar expr tt, expr tt) for tt in l]]

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
        [sexpr(-"pset"), [sexpr(v) for v in fv ps], [[sexpr k, sexpression term] for (k, term) in rep(ps).tbl]]

    parseSExpression(sx: SExpression): % ==
        p(e: SExpression): TypeTerm ==
	    stdout << "parseParam " << e << newline
	    parseSExpression(e)$TypeTerm
        [p e for e in sx]
	
TestTypeTerm: with
    testEquality: () -> ()
    testParamSet: () -> ()
== add
    import from TypeTerm
    import from Assert TypeTerm
    import from Symbol
    import from Partial TypeTerm
    import from ListSet Symbol    
    import from Assert ListSet Symbol

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
        testParamSet1()
        testParamSet2()

    local testParamSet1(): () == 
        ps1: ParamSet := param(-"x", fromString "(forall (a) a)")
	assertTrue(not failed? find(-"x", ps1))

    local testParamSet2(): () == 
        ps1: ParamSet := fromString "()"
	assertTrue(empty? ps1)
	ps1 := fromString "((param a x))"
	assertFalse(empty? ps1)
	assertEquals([-"a"], vars ps1)

#if ALDORTEST
#include "types"
#pile
import from TestTypeTerm
testEquality()
testParamSet()
#endif
