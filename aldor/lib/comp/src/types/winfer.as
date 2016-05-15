#include "comp.as"
#include "aldorio.as"
#pile

Exp: PrimitiveType with
    SExpressionType
    case: (%, 'var') -> Boolean
    case: (%, 'lit') -> Boolean
    case: (%, 'abs') -> Boolean
    case: (%, 'app') -> Boolean
    case: (%, 'bind') -> Boolean
    case: (%, '_if') -> Boolean
    case: (%, '_let') -> Boolean
    case: (%, 'letrec') -> Boolean

    var: % -> Symbol
    lit: % -> Lit
    abs: % -> (Symbol, %)
    app: % -> (%, %)
    bind: % -> List Cross(Symbol, %)
    _if: % -> (%, %, %)
    _let: % -> (Symbol, %, %)
    letrec: % -> (%, %)
        
    parse: SExpression -> %
    export from 'var','lit','abs','app','bind','_if','_let','letrec'
== add
    Rep == Union(var: Symbol,
    	   	 lit: Lit,
    	   	 app: Cross(%, %),
		 abs: Cross(Symbol, %),
		 bind: List Cross(Symbol, %),
		 _if: Cross(%, %, %),
		 letrec: Cross(%, %),
		 _let: Cross(Symbol, %, %))

    import from Rep
    import from String
    
    (exp: %) case (v: 'var'): Boolean == rep(exp) case var
    (exp: %) case (v: 'lit'): Boolean == rep(exp) case lit
    (exp: %) case (v: 'abs'): Boolean == rep(exp) case abs
    (exp: %) case (v: 'bind'): Boolean == rep(exp) case bind
    (exp: %) case (v: '_if'): Boolean == rep(exp) case _if
    (exp: %) case (v: 'app'): Boolean == rep(exp) case app
    (exp: %) case (v: '_let'): Boolean == rep(exp) case _let
    (exp: %) case (v: 'letrec'): Boolean == rep(exp) case letrec

    var(exp: %): Symbol == rep(exp).var
    lit(exp: %): Lit == rep(exp).lit
    abs(exp: %): (Symbol, %) == rep(exp).abs
    app(exp: %): (%, %) == rep(exp).app
    bind(exp: %): List Cross(Symbol, %) == rep(exp).bind
    _if(exp: %): (%, %, %) == rep(exp)._if
    _let(exp: %): (Symbol, %, %) == rep(exp)._let
    letrec(exp: %): (%, %) == rep(exp).letrec

    (e1: %) = (e2: %): Boolean ==
        e1 case var => e2 case var and var e1 = var e2
        e1 case lit => e2 case lit and lit e1 = lit e2
        e1 case abs => e2 case abs and absEqual(e1, e2)
        e1 case app => e2 case app and appEqual(e1, e2)
        e1 case bind => e2 case bind and bindEqual(e1, e2)
        e1 case _if => e2 case _if and ifEqual(e1, e2)
        e1 case _let => e2 case _let and letEqual(e1, e2)
        e1 case letrec => e2 case _let and letRecEqual(e1, e2)
	never

    local absEqual(e1: %, e2: %): Boolean ==
        (v1, i1) := abs e1
        (v2, i2) := abs e2
	v1 = v2 and i1 = i2

    local appEqual(e1: %, e2: %): Boolean ==
        (v1, i1) := app e1
        (v2, i2) := app e2
	v1 = v2 and i1 = i2

    local bindEqual(e1: %, e2: %): Boolean ==
        import from BooleanFold, MachineInteger
        l1 := bind e1
        l2 := bind e2 
	#l1 = #l2 and (_and)/(v1 = v2 and e1 = e2 for (v1, d1) in l1 for (v2, d2) in l2)

    local letEqual(e1: %, e2: %): Boolean ==
        (v1, d1, c1) := _let e1
        (v2, d2, c2) := _let e2
	v1 = v2 and d1 = d2 and c1 = c2

    local letRecEqual(e1: %, e2: %): Boolean ==
        (d1, b1) := letrec e1
        (d2, b2) := letrec e2
	d1 = d2 and b1 = b2

    local ifEqual(e1: %, e2: %): Boolean ==
        (v1, d1, c1) := _if e1
        (v2, d2, c2) := _if e2
	v1 = v2 and d1 = d2 and c1 = c2

    sexpression(exp: %): SExpression ==
        import from Symbol
        select exp in
	    var => sexpr var exp
	    lit => sexpression lit exp
	    abs =>
	        (sym, body) := abs exp
	        [sexpr(-"abs"), sexpr sym, sexpression body]
	    app =>
	        (op, arg) := app exp
		[sexpression op, sexpression arg]
	    bind =>
	        [[sexpr var, sexpression val] for (var, val) in bind exp]
	    _if =>
	        (t, conseq, anti) := _if exp 
		[sexpr(-"if"), sexpression t, sexpression conseq, sexpression anti]
	    _let =>
	        (lvar, def, iexp) := _let exp
		[sexpr(-"let"), [[sexpr lvar, sexpression def]], sexpression iexp]
	    letrec =>
	        (def, body) := letrec exp
		[sexpr(-"letrec"), sexpression def, sexpression body]
	    error("dunno")
	    
    parse(sx: SExpression): % ==
        stdout << "(P: " << sx << newline
	p := parse0 sx
	stdout << " " << p << ")" << newline
	return p
	
    parse0(sx: SExpression): % ==
        stdout << "Parse: " << sx << newline
        import from Lit, Symbol, Integer
	int? sx => per [litInteger toString int sx]
        sym? sx => per [sym sx]
	cons? sx =>
	    if sym? first sx and special? sym first sx then
	        parseSpecial(sx)
	    else
	        not nil? rest rest sx => error("Too many arguments to apply " + toString sx)
	        per [app == (parse first sx, parse first rest sx)]
	error("sx: " + toString sx + " ")

    local specialSymbols: Set Symbol == [-"lambda", -"let", -"if", -"letrec"]

    local special?(sym: Symbol): Boolean ==
        stdout << "special " << sym << " " << member?(sym, specialSymbols) << newline
        member?(sym, specialSymbols)

    local parseSpecial(sx: SExpression): % ==
        import from Symbol, Integer
        stdout << "Special: " << sym first sx << " " << sx << newline
	op := sym first sx
	op = -"lambda" =>
	    per [abs == (sym first rest sx, parse first rest rest sx)]
	op = -"if" =>
	    per [_if == (parse first rest sx, parse first rest rest sx, parse first rest rest rest sx)]
	op = -"let" =>
	    defs := first rest sx
	    stdout << "Def0 " << defs << newline
	    def0 := nth(defs, 0)
	    lhs := sym first def0
	    rhs := nth(def0, 1)
	    body := nth(sx, 2)
	    per [_let == (lhs, parse rhs, parse body)]
	op = -"letrec" =>
	    bindings := nth(sx, 1)
	    body := nth(sx, 2)
	    per [letrec == (parseBindings bindings, parse body)]
	op = -"if" =>
	    tst := nth(sx, 0)
	    conseq := nth(sx, 1)
	    anti := nth(sx, 2)
	    per [_if == (parse tst, parse conseq, parse anti)]
	error("How! " + toString sx)

    local parseBindings(sx: SExpression): % ==
        import from List Cross(Symbol, %), Integer
        per [bind == [(sym first def, parse(nth(def, 1))) for def in sx]]

Lit: PrimitiveType with
    SExpressionType
    case: (%, 'int') -> Boolean
    case: (%, 'bool') -> Boolean
    int: % -> String
    bool: % -> String

    litBoolean: String -> %
    litInteger: String -> %
== add
    Rep == Union(int: String, bool: String)
    import from Rep, 'int','bool'
    
    (lit: %) case (v: 'int'): Boolean == rep(lit) case int
    (lit: %) case (v: 'bool'): Boolean == rep(lit) case bool

    int(lit: %): String == rep(lit).int
    bool(lit: %): String == rep(lit).bool

    litInteger(s: String): % == per [int == s]
    litBoolean(s: String): % == per [bool == s]

    sexpression(lit: %): SExpression ==
        import from Symbol
	select lit in
            int => [sexpr(-"int"), sexpr int lit]
            bool => [sexpr(-"bool"), sexpr bool lit]
	    never
	
    (a: %) = (b: %): Boolean == never

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

    tvar: % -> Symbol
    map: % -> (%, %)

    typeMap: (%, %) -> %
    typeTVar: Symbol -> %
    bool: %
    int: %
    export from 'tvar','int','bool','map'
== add
    Rep ==> Union(tvar: Symbol, prim: 'int,bool', map: Cross(%, %))
    import from Rep, 'int,bool'
    import from 'tvar','int','bool','map'

    typeMap(op: %, arg: %): % == per [(op, arg)@Cross(%, %)]
    typeTVar(sym: Symbol): % == per [sym]
    int: % == per [int]
    bool: % == per [bool]

    (type: %) case (v: 'tvar'): Boolean == rep(type) case tvar
    (type: %) case (v: 'int'): Boolean == rep(type) case prim and rep(type).prim = int
    (type: %) case (v: 'bool'): Boolean == rep(type) case prim and rep(type).prim = bool
    (type: %) case (v: 'map'): Boolean == rep(type) case map

    tvar(type: %): Symbol == rep(type).tvar
    map(type: %): (%, %) == rep(type).map

    (a: %) = (b: %): Boolean == 
        a case tvar and b case tvar => tvar a = tvar b
	a case int and b case int => true
	a case bool and b case bool => true
	a case map and b case map =>
	    (aop, aarg) := map a
	    (bop, barg) := map b
	    aop = bop and aarg = barg
	false
	
    ftv(t: %): Set Symbol == select t in
        tvar => [tvar t]
	int => empty
	bool => empty
	map =>
	    (op, arg) := map t
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
	        typeMap(apply(s, op), apply(s, arg))
	    t


    sexpression(t: %): SExpression ==
        import from Symbol
        select t in
	    tvar => [sexpr(-"tvar"), sexpr tvar t]
	    int => [sexpr(-"int")]
	    bool => [sexpr(-"bool")]
	    map =>
	        (op, arg) := map t
	        [sexpr(-"map"), sexpression op, sexpression arg]
	    never

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

    (a: %) = (b: %): Boolean == never

    sexpression(scheme: %): SExpression ==
        import from TType, Symbol, Set Symbol
	sx: SExpression := sexpression(type(scheme))$TType
        [sexpr(-"scheme"), [sexpr v for v in vars scheme], sexpression(type(scheme))]
	
Subst: PrimitiveType with
    SExpressionType
    null: %
    *: (%, %) -> %
    lookup: (%, Symbol) -> Partial TType
    remove: (Set Symbol, %) -> %
    substOne: (Symbol, TType) -> %
    subst: Generator Cross(Symbol, TType) -> %
    subst: Tuple Cross(Symbol, TType) -> %
== add
    Rep == HashTable(Symbol, TType)
    import from Rep
    null: % == per []

    substOne(s: Symbol, type: TType): % == per [(s, type)@Cross(Symbol, TType)]
    subst(g: Generator Cross(Symbol, TType)): % == per [g]
    subst(t: Tuple Cross(Symbol, TType)): % ==
        l: List Cross(Symbol, TType) := [t]
        per [c for c in l]

    (s1: %) * (s2: %): % ==
        import from TType
        tbl := [(var, apply(s1, type)) for (var, type) in rep s2]
	for (var, type) in rep s1 repeat tbl.var := type
	per tbl
	
    lookup(sigma: %, s: Symbol): Partial TType ==
        find(s, rep(sigma))

    remove(syms: Set Symbol, sigma: %): % ==
        import from Partial TType
        commonSyms := [sym for sym in syms | not failed? find(sym, rep(sigma))]
	empty? commonSyms => sigma
	per [(k, v) for (k, v) in rep(sigma) | not member?(k, commonSyms)]

    (a: %) = (b: %): Boolean == rep(a) = rep(b)

    sexpression(sigma: %): SExpression ==
        import from Symbol, TType
        cons(sexpr(-"subst"), [[sexpr sym, sexpression type] for (sym, type) in rep sigma])
	
TypeEnv: PrimitiveType with
    SExpressionType
    TypeFns %
    remove: (%, Symbol) -> %
    generalise: (%, TType) -> Scheme
    lookup: (%, Symbol) -> Partial Scheme
    empty: () -> %
    singleton: (Symbol, Scheme) -> %
    union: (%, %) -> %
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
        scheme(vars, type)

    apply(sigma: Subst, env: %): % == per [(var, apply(sigma, sch)) for (var, sch) in rep(env)]
    ftv(env: %): Set Symbol == (union, empty)/(ftv type for (k, type) in rep(env))

    union(e1: %, e2: %): % ==
        import from Generator Cross(Symbol, Scheme)
        import from List Generator Cross(Symbol, Scheme)
        per [concat [generator rep e1, generator rep e2]]

    (a: %) = (b: %): Boolean == rep(a) = rep(b)
    
    sexpression(env: %): SExpression == -- sexpr "env"
        import from Symbol
	cons(sexpr(-"env"),
     	     [[sexpr sym, sexpression val] for (sym, val) in rep env])

State: with
    newTypeVar: () -> TType
== add
    import from Symbol
    newTypeVar(): TType == typeTVar new()

WTypeInfer: with
    ti: (TypeEnv, Exp) -> (Subst, TType)
    typeInfer: (TypeEnv, Exp) -> TType
== add
    import from State
    import from String

    typeInfer(env: TypeEnv, e: Exp): TType ==
        (s, t) := ti(env, e)
	apply(s, t)

    unify(t1: TType, t2: TType): Subst ==
        t1 case map and t2 case map =>
            (op1, arg1) := map t1
            (op2, arg2) := map t2
	    s1 := unify(op1, op2)
	    s2 := unify(apply(s1, arg1), apply(s1, arg2))
	    s1 * s2 -- NB: Possibly wrong!
        t1 case tvar => varBind(tvar t1, t2)
        t2 case tvar => varBind(tvar t2, t1)
	t1 case int and t2 case int => null
	t1 case bool and t2 case bool => null
	error "Failed to unify"

    varBind(sym: Symbol, type: TType): Subst ==
        import from Set Symbol
        type case tvar and tvar type = sym => null
        member?(sym, ftv type) => error("already bound: " + toString sym + toString type)
	substOne(sym, type)

    tiLit(e: TypeEnv, lit: Lit): (Subst, TType) ==
        select lit in
	    int => (null, int)
	    bool => (null, bool)
	    never

    ti(env: TypeEnv, exp: Exp): (Subst, TType) ==
        stdout << "(ti: " << env << " " << exp << newline
	(x, y) := ti1(env, exp)
	stdout << exp << " --> " << x << " " << y << ")" << newline
	(x, y)
	
    ti1(env: TypeEnv, exp: Exp): (Subst, TType) ==
        import from Partial Scheme, Scheme, Fold2 TypeEnv
        select exp in
            var =>
	        curr := lookup(env, var exp)
	    	failed? curr => error("unbound variable: " + toString(exp))
	        (null, instantiate retract curr)
            lit =>
	        tiLit(env, lit exp)
	    abs =>
	        (n, body) := abs exp
		tv := newTypeVar()
		env2 := remove(env, n)
		env3 := union(env, singleton(n, scheme(tv)))
		(s1, t1) := ti(env3, body)
		(s1, typeMap(apply(s1, tv), t1))
	    app =>
	        tv := newTypeVar()
	        (e1, e2) := app exp
	        (s1, t1) := ti(env, e1)
		(s2, t2) := ti(apply(s1, env), e2)
		s3 := unify(apply(s2, t1), typeMap(t2, tv))
		(s3 * s2 * s1, apply(s3, tv))

	    _if =>
	        (tst, conseq, anti) := _if exp
		(stst, ttst) := ti(env, tst)

		stst2 := unify(ttst, bool)
		stdout << "stst: " << stst << newline
		stdout << "stst2: " << stst2 << newline
		stdout << "ttst: " << ttst << newline

		env := apply(stst2, env)
		stdout << "env1: " << env << newline
		
		(sconseq, tconseq) := ti(env, conseq)
		stdout << "sconseq: " << sconseq << newline
		stdout << "tconseq: " << tconseq << newline

		env := apply(sconseq, env)
		stdout << "env3: " << env << newline

		(santi, tanti) := ti(env, anti)
		sfinal := unify(apply(sconseq, tconseq), apply(santi, tanti))
		stdout << "Final " << sfinal << newline
		(sfinal * santi * sconseq * stst2 * stst, tanti)
	    _let =>
	        (x, e1, e2) := _let exp
		(s1, t1) := ti(env, e1)
		env2 := remove(env, x)
		tx := generalise(apply(s1, env), t1)
		env3 := union(env, singleton(x, tx))
		(s2, t2) := ti(apply(s1, env3), e2)
		(s1 * s2, t2)
	    letrec =>
	        import from Fold2(Symbol, TypeEnv)
	        import from Fold2(TypeEnv)
	        import from List Cross(Symbol, Exp)
		import from Symbol
	        (bindings, body) := letrec exp
		typevars: List Cross(TType, Symbol, Exp) := [(newTypeVar(), var, decl) for (var, decl) in bind bindings]
		env0 := (remove, env)/(id for (tvar, id, decl) in typevars)
		env := (union, env)/(singleton(var, scheme(tvar)) for (tvar, var, decl) in typevars)
		varTypes: HashTable(Symbol, TType) := []
		varSubsts: HashTable(Symbol, Subst) := []
		fullEnv := env0

		for (var, def) in bind bindings repeat
		    (sn, tn) := ti(env, def)
		    stdout << "var: " << var << " S: " << sn << newline
		    stdout << "var: " << var << " T: " << tn << newline
		    varTypes.var := tn
		    varSubsts.var := sn

		fullSubst := subst()
		for (var, def) in bind bindings repeat
		    fullSubst := varSubsts.var * fullSubst

		stdout << "Full: " << fullSubst << newline
                for (var, type) in varTypes repeat
		    stdout << var << ": " << type << newline
		    stdout << var << ": " << fullSubst.type << newline
		
		for (tvar, var, decl) in typevars repeat
		    s := unify(fullSubst.tvar, fullSubst.(varTypes.var))
		    stdout << var << " U: " << s << newline
		    fullSubst := s * fullSubst
		    stdout << var << " " << fullSubst.tvar << newline
		    
		fullEnv := (union, env0)/(singleton(var, generalise(env0, fullSubst tvar)) for (tvar, var, decl) in typevars)
		stdout << "Full Subst: " << fullSubst << newline
		stdout << "Full Env: " << fullEnv << newline
		(sfinal, tfinal) := ti(fullSubst.fullEnv, body)
	    never

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile
import from Assert Exp, Assert TType
import from String, Partial SExpression, SExpression
import from Symbol

testLetRec(): () ==
    prog := retract readOne "(letrec ((x 1) (y 2)) x)"
    e: Exp := parse prog
    (defs, body) := letrec e
    assertTrue(e case letrec)
    assertTrue(defs case bind)
    assertTrue(body case var)

testSubst1(): () ==
    import from TType, Subst, Symbol
    import from Assert TType
    tf := typeTVar(-"A")
    subst  := substOne(-"A", typeTVar(-"B"))
    assertEquals(subst tf, typeTVar(-"B"))

testSubst2(): () ==
    import from TType, Subst, Symbol
    import from Assert TType
    tf := typeTVar(-"A")
    subst  := substOne(-"A", int)
    assertEquals(subst tf, int)

    tf := typeTVar(-"B")
    subst  := substOne(-"A", int)
    assertEquals(subst tf, tf)

testSubst3(): () ==
    import from TType, Subst, Symbol
    import from Assert TType
    tf := typeTVar(-"A")
    subst1  := substOne(-"B", typeTVar(-"C"))
    subst2  := substOne(-"A", typeTVar(-"B"))
    composition := subst1 * subst2
    stdout << "Composition: " << composition << newline
    assertEquals(composition typeTVar(-"A"), typeTVar(-"C"))
    assertEquals(composition typeTVar(-"B"), typeTVar(-"C"))
    assertEquals(composition typeTVar(-"X"), typeTVar(-"X"))

testSubst(): () ==
    testSubst1()
    testSubst2()
    testSubst3()

testSubst()

readOne(s: String): Partial SExpression ==
    import from SExpressionReader
    sb: StringBuffer := new()
    sb::TextWriter << s
    read(sb::TextReader)

test1(): () ==
    prog := retract readOne("v")
    e: Exp := parse prog
    assertTrue(e case var);
    assertTrue(var e = -"v");

test2(): () ==
    prog := retract readOne "(lambda x (inc x))"
    stdout << "Prog : " << prog << newline
    e: Exp := parse prog
    stdout << "Parsed: " << e << newline
    assertTrue(e case abs)
    (var, body) := abs e
    assertTrue(var = -"x")
    assertTrue(body case app)

test3(): () ==
    prog := retract readOne "(let ((x 22)) x)"
    stdout << "Prog : " << prog << newline
    e: Exp := parse prog
    stdout << "Parsed: " << e << newline
    assertTrue(e case _let)
    (var, def, body) := _let e 
    assertTrue(var = -"x")
    assertTrue(body case var)


test4(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(lambda x x)"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)


test5(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(lambda x 2)"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)

test6(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(let ((x 0)) (lambda y x))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)

test7(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(let ((f (lambda x 1))) (f 0))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case int)

test8(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(let ((f (lambda x 1))) (let ((g 3)) (f g))))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case int)

test9(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(lambda a (let ((x (lambda b (let ((y (lambda c (a 1)))) (y 2))))) (x 3)))"
    (subst, tp) := ti(empty(), prog)
    rr := apply(subst, tp)
    stdout << "Result: " << rr << newline
    assertTrue(tp case map)

test10(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(lambda a (lambda b (b (a (a b)))))"
    (subst, tp) := ti(empty(), prog)
    rr := apply(subst, tp)
    stdout << "Result: " << rr << newline
    assertTrue(tp case map)

run(): () ==
    test1()
    test2()
    test3()
    test4()
    test5()
    test6()
    test7()
    test8()
    test9()
    test10()
    
run()

testIf1(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(lambda a (lambda b (if a b 1)))"
    (subst, tp) := ti(empty(), prog)
    rr := apply(subst, tp)
    stdout << "Result: " << rr << newline
    assertEquals(rr, typeMap(bool, typeMap(int, int)))

testIf1()
testLetRec()

testLetRec2(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec () 1)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec3(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((x 1)) x)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

exampleEnv(): TypeEnv ==
    import from Fold2 TypeEnv, Symbol, Scheme, TType, List TypeEnv
    (union, empty())/[singleton(-"add", scheme(typeMap(int, typeMap(int, int)))),
    	    	      singleton(-"negate", scheme(typeMap(int, int))),
    	    	      singleton(-"dec", scheme(typeMap(int, int))),
    	    	      singleton(-"zero?", scheme(typeMap(int, bool)))]

testLetRec4(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((x 1)) x)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec5(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((x 1) (y 2)) ((add x) y))"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst.tp, int)

testLetRec6(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((factorial (lambda a (negate (factorial (dec a)))))) (factorial 4))"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst.tp, int)

testLetRec7(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((x y) (y 3)) x)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec8(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog := parse retract readOne "(letrec ((f (lambda x (g x))) (g (lambda x (dec (f x))))) (f 2))"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst.tp, int)

testLetRec2()
testLetRec3()
testLetRec4()
testLetRec5()
testLetRec6()
testLetRec7()
testLetRec8()

#endif
