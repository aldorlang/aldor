#include "comp.as"
#include "aldorio.as"
#pile

TypeFns(T: PrimitiveType): Category == with
    ftv: T -> Set Symbol
    apply: (Subst, T) -> T

TType: PrimitiveType with
    SExpressionType
    TypeFns %
    case: (%, 'tvar') -> Boolean
    case: (%, 'int') -> Boolean
    case: (%, 'bool') -> Boolean
    case: (%, 'map') -> Boolean

    case: (%, 'id') -> Boolean
    case: (%, 'app') -> Boolean

    tvar: % -> Symbol
    map: % -> (%, %)
    app: % -> (%, %)

    unify: (%, %) -> Subst

    typeMap: (%, %) -> %
    typeTVar: Symbol -> %
    bool: %
    int: %
    typeId: Symbol -> %
    typeApp: (%, %) -> %

    export from 'tvar','int','bool','map', 'id', 'app'
== add
    Rep ==> Union(tvar: Symbol,
    	          prim: 'int,bool',
		  map: Cross(%, %),
		  id: Symbol,
		  app: Cross(%, %))
    import from Rep, 'int,bool'
    import from 'tvar','int','bool','map'

    typeMap(op: %, arg: %): % == per [map==(op, arg)@Cross(%, %)]
    typeTVar(sym: Symbol): % == per [tvar==sym]
    typeApp(op: %, arg: %): % == per [app==(op, arg)@Cross(%, %)]
    typeId(sym: Symbol): % == per [id==sym]
    int: % == per [int]
    bool: % == per [bool]

    (type: %) case (v: 'tvar'): Boolean == rep(type) case tvar
    (type: %) case (v: 'int'): Boolean == rep(type) case prim and rep(type).prim = int
    (type: %) case (v: 'bool'): Boolean == rep(type) case prim and rep(type).prim = bool
    (type: %) case (v: 'map'): Boolean == rep(type) case map
    (type: %) case (v: 'app'): Boolean == rep(type) case app
    (type: %) case (v: 'id'): Boolean == rep(type) case id

    tvar(type: %): Symbol == rep(type).tvar
    map(type: %): (%, %) == rep(type).map
    app(type: %): (%, %) == rep(type).app
    id(type: %): Symbol == rep(type).id

    (a: %) = (b: %): Boolean == 
        a case tvar and b case tvar => tvar a = tvar b
	a case int and b case int => true
	a case bool and b case bool => true
	a case map and b case map =>
	    (aop, aarg) := map a
	    (bop, barg) := map b
	    aop = bop and aarg = barg
	a case id and b case id => id a = id b
	a case app and b case app =>
	    (lhsa, rhsa) := app a
	    (lhsb, rhsb) := app b
	    lhsa = lhsb and rhsa = rhsb
	false
	
    ftv(t: %): Set Symbol == select t in
        tvar => [tvar t]
	int => empty
	bool => empty
	map =>
	    (arg, ret) := map t
	    union(ftv arg, ftv ret)
	id => empty
	app =>
	    (op, arg) := app t
	    union(ftv op, ftv arg)
	never

    apply(s: Subst, t: %): % ==
        import from Partial %
        select t in 
            tvar =>
	        vv := lookup(s, tvar t)
		if failed? vv then t else retract vv
	    map =>
	        (op, arg) := map t
	        typeMap(s op, s arg)
            app =>
	        (op, arg) := app t
		typeApp(s op, s arg)
	    t


    unify(t1: %, t2: %): Subst ==
        s := unify1(t1, t2)
	stdout << "Unify " << t1 << " " << t2 << " --> " << s << newline
	s
	
    unify1(t1: %, t2: %): Subst ==
        t1 case map and t2 case map =>
            (op1, arg1) := map t1
            (op2, arg2) := map t2
	    s1 := unify(op1, op2)
	    s2 := unify(s1 arg1, s1 arg2)
	    s1 * s2 -- NB: Possibly wrong!
        t1 case tvar => varBind(tvar t1, t2)
        t2 case tvar => varBind(tvar t2, t1)
	t1 case int and t2 case int => null
	t1 case bool and t2 case bool => null
	t1 case id and t2 case id =>
	    if id t1 = id t2 then null else error("failed to unify")
	t1 case app and t2 case app =>
            (op1, arg1) := app t1
            (op2, arg2) := app t2
            s1 := unify(op1, op2)
	    s2 := unify(s1 arg1, s1 arg2)
	    s1 * s2
	error "Failed to unify"

    varBind(sym: Symbol, type: TType): Subst ==
        import from OutputTypeFunctions %
        import from Set Symbol
        type case tvar and tvar type = sym => null
        member?(sym, ftv type) => error("already bound: " + toString sym + toString type)
	substOne(sym, type)

    sexpression(t: %): SExpression ==
        import from Symbol
        select t in
	    tvar => [sexpr(-"tvar"), sexpr tvar t]
	    int => [sexpr(-"int")]
	    bool => [sexpr(-"bool")]
	    map =>
	        (op, arg) := map t
	        [sexpr(-"map"), sexpression op, sexpression arg]
	    app =>
	        (op, arg) := app t
	        [sexpr(-"app"), sexpression op, sexpression arg]
	    id => [sexpr(-"id"), sexpr id t]
	    never

Subst: PrimitiveType with
    SExpressionType
    null: %
    *: (%, %) -> %
    lookup: (%, Symbol) -> Partial TType
    remove: (Set Symbol, %) -> %
    substOne: (Symbol, TType) -> %
    subst: Generator Cross(Symbol, TType) -> %
    subst: Tuple Cross(Symbol, TType) -> %
    failed?: % -> Boolean
    fail: String -> %
    vars: % -> Set Symbol
== add
    Rep == Union(failed: String, tbl: HashTable(Symbol, TType))
    import from HashTable(Symbol, TType)
    import from Rep
    null: % == per [tbl==[]]

    local tbl(sigma: %): HashTable(Symbol, TType) == rep(sigma).tbl

    failed?(sigma: %): Boolean == rep(sigma) case failed
    fail(msg: String): % == per [msg]
    message(sigma: %): String == rep(sigma).failed
    vars(sigma: %): Set Symbol == [sym for (sym, tp) in tbl sigma]

    substOne(s: Symbol, type: TType): % == per [[(s, type)@Cross(Symbol, TType)]]
    subst(g: Generator Cross(Symbol, TType)): % == per [[g]]
    subst(t: Tuple Cross(Symbol, TType)): % ==
        l: List Cross(Symbol, TType) := [t]
        per [[c for c in l]]

    (s1: %) * (s2: %): % ==
        failed? s1 and failed? s2 => fail(message s1 + " - " + message s2)
        failed? s1 => s1
        failed? s2 => s2
        import from TType
        tt := [(var, apply(s1, type)) for (var, type) in tbl s2]
	for (var, type) in tbl s1 repeat tt.var := type
	per [tt]
	
    lookup(sigma: %, s: Symbol): Partial TType ==
        failed? sigma => failed
        find(s, tbl(sigma))

    remove(syms: Set Symbol, sigma: %): % ==
        import from Partial TType
	failed? sigma => sigma
        commonSyms := [sym for sym in syms | not failed? find(sym, tbl(sigma))]
	empty? commonSyms => sigma
	per [[(k, v) for (k, v) in tbl(sigma) | not member?(k, commonSyms)]]

    (a: %) = (b: %): Boolean ==
        failed? a => failed? b
	failed? b => false
	tbl a = tbl b

    sexpression(sigma: %): SExpression ==
        import from Symbol, TType
	failed? sigma => [sexpr(-"subst-failed"), sexpr message sigma]
        cons(sexpr(-"subst"), [[sexpr sym, sexpression type] for (sym, type) in tbl sigma])

-- A scheme is a type, plus a set of bound variables
Scheme: PrimitiveType with
    SExpressionType
    TypeFns %
    vars: % -> Set Symbol
    type: % -> TType

    scheme: (Set Symbol, TType) -> %
    scheme: (TType) -> %
    instantiate: % -> TType
== add
    Rep == Record(vars: Set Symbol, type: TType)
    import from Rep
    import from State
    
    vars(scheme: %): Set Symbol == rep(scheme).vars
    type(scheme: %): TType == rep(scheme).type

    scheme(t: TType): % == scheme([], t)
    scheme(s: Set Symbol, t: TType): % == per [s, t]

    ftv(scheme: %): Set Symbol == [sym for sym in ftv type scheme | not member?(sym, vars scheme)]
    apply(sigma: Subst, s: %): % == scheme(vars s, apply(remove(vars s, sigma), type s))

    instantiate(scheme: %): TType ==
        import from Subst
        sigma := subst((var, newTypeVar()) for var in vars scheme)
	apply(sigma, type scheme)

    (a: %) = (b: %): Boolean ==
        import from MachineInteger
	import from TType, Subst, BooleanFold
        # vars a ~= #vars b => false
	s1 := unify(type a, type b)
	failed? s1 => false
	not((_and)/(member?(var, vars a) for var in vars s1)) => false
	s2 := unify(type b, type a)
	failed? s2 => false
	not(_and/(member?(var, vars b) for var in vars s2)) => false
	true
	
    sexpression(scheme: %): SExpression ==
        import from TType, Symbol, Set Symbol
	sx: SExpression := sexpression(type(scheme))$TType
        [sexpr(-"scheme"), [sexpr v for v in vars scheme], sexpression(type(scheme))]
	
	
TypeEnv: PrimitiveType with
    SExpressionType
    TypeFns %
    remove: (%, Symbol) -> %
    generalise: (%, TType) -> Scheme
    generaliseTop: TType -> Scheme
    lookup: (%, Symbol) -> Partial Scheme
    empty: () -> %
    singleton: (Symbol, Scheme) -> %
    union: (%, %) -> %
    generator: % -> Generator Cross(Symbol, Scheme)
== add
    Rep == HashTable(Symbol, Scheme)
    import from Rep, Scheme
    import from Fold2 Set Symbol

    empty(): % == per []
    singleton(s: Symbol, type: Scheme): % ==
        pair := (s, type)
        per [pair]
    remove(env: %, sym: Symbol): % == per [(var, sch) for (var, sch) in rep env | var ~= sym]

    lookup(env: %, sym: Symbol): Partial Scheme ==
        stdout << "Lookup " << sym << " " << env << newline
        find(sym, rep(env))

    generalise(env: %, type: TType): Scheme ==
        vars: Set Symbol := [sym for sym in ftv type| not member?(sym, ftv env)]
	stdout << "generalise: " << ftv env << type << " ---> " << vars << newline
        scheme(vars, type)

    generaliseTop(type: TType): Scheme == scheme(ftv type, type)

    apply(sigma: Subst, env: %): % == per [(var, apply(sigma, sch)) for (var, sch) in rep(env)]
    import from Symbol
    ftv(env: %): Set Symbol == (union, empty)/(ftv type for (k, type) in rep(env))

    union(e1: %, e2: %): % ==
        import from Generator Cross(Symbol, Scheme)
        import from List Generator Cross(Symbol, Scheme)
        per [concat [generator rep e1, generator rep e2]]

    (a: %) = (b: %): Boolean == rep(a) = rep(b)
    
    generator(env: %): Generator Cross(Symbol, Scheme) == generator rep env

    sexpression(env: %): SExpression == -- sexpr "env"
        import from Symbol
	cons(sexpr(-"env"),
     	     [[sexpr sym, sexpression val] for (sym, val) in rep env])

State: with
    newTypeVar: () -> TType
== add
    import from Symbol
    newTypeVar(): TType == typeTVar new()
