#include "comp.as"
#include "aldorio.as"
#pile


WTypeInfer: with
    ti: (TypeEnv, Exp) -> (Subst, TType)
    typeInfer: (TypeEnv, Exp) -> TType
== add
    import from State
    import from String

    typeInfer(env: TypeEnv, e: Exp): TType ==
        (s, t) := ti(env, e)
	apply(s, t)

    tiLit(e: TypeEnv, lit: Lit): (Subst, TType) ==
        select lit in
	    int => (null, int)
	    bool => (null, bool)
	    never

    ti(env: TypeEnv, exp: Exp): (Subst, TType) ==
--        stdout << "(ti: " << env << " " << exp << newline
	(x, y) := ti1(env, exp)
--	stdout << exp << " --> " << x << " " << y << ")" << newline
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
		stdout << "abs: " << tv << newline
		stdout << "abs: " << t1 << newline
		stdout << "abs: " << s1 << newline
		stdout << "abs: " << s1 typeMap(s1 tv, t1) << newline
		(s1, typeMap(s1 tv, t1))
	    app =>
	        tv := newTypeVar()
	        (e1, e2) := app exp
	        (s1, t1) := ti(env, e1)
		(s2, t2) := ti(apply(s1, env), e2)
		s3 := unify(apply(s2, t1), typeMap(t2, tv))
		stdout << "app " << s3 tv << newline
		stdout << "app " << s3 * s2 * s1 << newline
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
		stdout << "Final type " << tanti << newline
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
		    stdout << "Subst result " << var << ": " << type << newline
		    stdout << "Subst result " << var << ": " << fullSubst.type << newline
		
		for (tvar, var, decl) in typevars repeat
		    s := unify(fullSubst.tvar, fullSubst.(varTypes.var))
		    stdout << "Unify: " << var << " U: " << s << newline
		    fullSubst := s * fullSubst
		    stdout << var << " " << fullSubst.tvar << newline
		    
		fullEnv := (union, env0)/(singleton(var, generalise(fullSubst env0, fullSubst tvar)) for (tvar, var, decl) in typevars)
		stdout << "Full Subst: " << fullSubst << newline
		stdout << "Full Env: " << fullEnv << newline
		(sfinal, tfinal) := ti(fullSubst.fullEnv, body)
		(sfinal * fullSubst, sfinal fullSubst tfinal)
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
    import from Fold2 TypeEnv, Symbol, Scheme, TType, List TypeEnv, Set Symbol, Symbol
    any := typeTVar(-"A")
    listAny := typeApp(typeId(-"List"), any)
    (union, empty())/[singleton(-"add", scheme(typeMap(int, typeMap(int, int)))),
    	    	      singleton(-"negate", scheme(typeMap(int, int))),
    	    	      singleton(-"dec", scheme(typeMap(int, int))),
		      singleton(-"zero?", scheme(typeMap(int, bool))),
		      singleton(-"cons", scheme([-"A"], typeMap(any, typeMap(listAny, listAny)))),
		      singleton(-"nil?", scheme([-"A"], typeMap(listAny, bool))),
		      singleton(-"nil", scheme([-"A"], listAny)),
		      singleton(-"first", scheme([-"A"], typeMap(listAny, any))),
		      singleton(-"rest", scheme([-"A"], typeMap(listAny, listAny)))]

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

testLambda(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda x (x 0))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << tp << newline
testLambda()

testLambda2(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda fn (letrec ((m (lambda l (if (nil? l) nil ((cons (fn (first l))) (m (rest l))))))) m))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
testLambda2()

testLambda0(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda x ((cons x) nil))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << tp << newline
--testLambda0()

testLambda3(): () ==
    import from WTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(letrec ((copy (lambda l (if (nil? l) nil ((cons (first l)) (copy (rest l))))))) copy)"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << tp << newline
--testLambda3()

#endif
