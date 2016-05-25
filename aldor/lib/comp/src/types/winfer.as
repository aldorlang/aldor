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
    e: Exp := fromString "(letrec ((x 1) (y 2)) x)"
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

test1(): () ==
    e: Exp := fromString "v"
    assertTrue(e case var);
    assertTrue(var e = -"v");

test2(): () ==
    e: Exp := fromString "(lambda x (inc x))"
    stdout << "Parsed: " << e << newline
    assertTrue(e case abs)
    (var, body) := abs e
    assertTrue(var = -"x")
    assertTrue(body case app)

test3(): () ==
    prog := "(let ((x 22)) x)"
    e: Exp := fromString prog
    stdout << "Parsed: " << e << newline
    assertTrue(e case _let)
    (var, def, body) := _let e 
    assertTrue(var = -"x")
    assertTrue(body case var)


test4(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(lambda x x)"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)


test5(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(lambda x 2)"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)

test6(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(let ((x 0)) (lambda y x))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case map)

test7(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(let ((f (lambda x 1))) (f 0))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case int)

test8(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(let ((f (lambda x 1))) (let ((g 3)) (f g))))"
    (subst, tp) := ti(empty(), prog)
    assertTrue(tp case int)

test9(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(lambda a (let ((x (lambda b (let ((y (lambda c (a 1)))) (y 2))))) (x 3)))"
    (subst, tp) := ti(empty(), prog)
    rr := apply(subst, tp)
    stdout << "Result: " << rr << newline
    assertTrue(tp case map)

test10(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(lambda a (lambda b (b (a (a b)))))"
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
    prog: Exp := fromString "(lambda a (lambda b (if a b 1)))"
    (subst, tp) := ti(empty(), prog)
    rr := apply(subst, tp)
    stdout << "Result: " << rr << newline
    assertEquals(rr, typeMap(bool, typeMap(int, int)))

testIf1()
testLetRec()

testLetRec2(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(letrec () 1)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec3(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString"(letrec ((x 1)) x)"
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
    prog: Exp := fromString "(letrec ((x 1)) x)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec5(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(letrec ((x 1) (y 2)) ((add x) y))"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst.tp, int)

testLetRec6(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(letrec ((factorial (lambda a (negate (factorial (dec a)))))) (factorial 4))"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst.tp, int)

testLetRec7(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(letrec ((x y) (y 3)) x)"
    (subst, tp) := ti(empty(), prog)
    assertEquals(subst.tp, int)

testLetRec8(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType
    prog: Exp := fromString "(letrec ((f (lambda x (g x))) (g (lambda x (dec (f x))))) (f 2))"
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
