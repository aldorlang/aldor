#include "types"
#include "aldorio"
#pile

XStream(X: with): with
    toXStream: (g: Generator X) -> %
    generator: % -> Generator X
== add
    Rep == Generator(X)
    generator(s: %): Generator X == rep s
    toXStream(g: Generator X): % == per g

Collector(T: with): with
    collect: % -> Finaliser(T, List T)
    newCollector: () -> %
== add
    import from Integer
    newCollector(): % == 0 pretend %
    collect(c: %): Finaliser(T, List T) ==
        listify(s: XStream T): List T == [v for v in s]
	finalise(listify)

Finaliser(T: with, V: with): with
    build: (%, XStream T) -> V
    finalise: (XStream T -> V) -> %
== add
    Rep ==> XStream T -> V
    build(f: %, s: XStream T): V ==
        fn: XStream T -> V := rep f
    	fn s

    finalise(fn: XStream T -> V): % == per fn

XStreamOp(T: with, V: with): with
    opFn: (%, T) -> Generator V
    streamOp: (T -> Generator V) -> %
== add
    Rep ==> T -> Generator V

    opFn(o: %, n: T): Generator V ==
        f := rep o
	f n

    streamOp(f: T -> Generator V): % == per f

StreamOperations(T: with, V: with): with
    >>: (XStream T, XStreamOp(T, V)) -> XStream V
    >>: (Generator T, XStreamOp(T, V)) -> XStream V
== add
    import from XStream T

    (>>)(s: XStream T, op: XStreamOp(T, V)): XStream V ==
        gg(): Generator V == generate
            for v in s repeat
                for v2 in opFn(op, v) repeat
	            yield v2
        toXStream(gg())

    (>>)(s: Generator T, op: XStreamOp(T, V)): XStream V == (toXStream s) >> op


FinalOperations(T: with, V: with): with
    >>: (XStream T, Finaliser(T, V)) -> V
    >>: (Generator T, Finaliser(T, V)) -> V
== add
    import from XStream T
    (>>)(s: XStream T, f: Finaliser(T, V)): V == build(f, s)
    (>>)(s: Generator T, f: Finaliser(T, V)): V == toXStream s >> f

GeneratorFun: with
    example: () -> ()
== add
    import from Integer
    import from List Integer
    import from List String
    import from Collector Integer
    import from StreamOperations(Integer, Integer)
    import from StreamOperations(String, String)
    import from StreamOperations(Integer, String)
    import from FinalOperations(Integer, Integer)
    import from FinalOperations(Integer, List Integer)
    import from FinalOperations(String, List String)

    example(): () ==
        g(): Generator Integer == x for x in 1..10
	b := g() >> collect(toList)
        h := g() >> filter(isEven?) >> map(double) >> collect(toList)
        i := g() >> filter(isEven?) >> map(toString) >> collect(toList)
        n := g() >> filter(isEven?) >> map(double) >> sum()
	stdout << "b " << b << newline
	stdout << "h " << h << newline
	stdout << "i " << i << newline
	stdout << "n " << n << newline
#if 0
    (s1: Generator Integer) >> (op: XStreamOp Integer): IntXStream == 
        g := (generate for x in s1 repeat for v in opFn(op, x) repeat yield v)
        g

    (s1: Generator Integer) >> (op: XStreamOp String): StringXStream == 
        g := (generate for x in s1 repeat for v in opFn(op, x) repeat yield v)
        g
#endif

    filter(f: Integer -> Boolean): XStreamOp(Integer, Integer) == 
        genOne(a: Integer): Generator Integer == generate if f(a) then yield a
        streamOp(genOne)
    
    map(f: Integer -> Integer): XStreamOp(Integer, Integer) == 
        genOne(a: Integer): Generator Integer == generate yield f(a)
        streamOp(genOne)
    
    map(f: Integer -> String): XStreamOp(Integer, String) == 
        genOne(a: Integer): Generator String == generate yield f(a)
        streamOp(genOne)
    
    toList: Collector Integer == newCollector()
    toList: Collector String == newCollector()

    isEven?(x: Integer): Boolean == x mod 2 = 0
    double(n: Integer): Integer == 2*n

    sum(): Finaliser(Integer, Integer) ==
        sumStream(s: XStream Integer): Integer ==
	    acc: Integer := 0
	    for v in s repeat acc := acc + v
	    return acc
	finalise(sumStream)


TestEnvs: with
    filterEnv: () -> Env
== add
    import from Symbol
    import from TypeTerm

    filterEnv(): Env ==
	env: Env := empty()
	put!(env, -"qq", fromString "(forall (t v) (apply map (comma (apply xstream t) (apply xstreamOp t v)) (apply xstream v)))")
	put!(env, -"qq", fromString "(forall (t v) (apply map (comma (apply generator t) (apply xstreamOp t v)) (apply xstream v)))")
	put!(env, -"qq", fromString "(forall (t v) (apply map (comma (apply generator t) (apply finaliser t v)) v))")
	put!(env, -"qq", fromString "(forall (t v) (apply map (comma (apply xstream t) (apply finaliser t v)) v))")
	put!(env, -"map", fromString "(forall (a b) (apply map (comma (apply map (comma a) b)) (apply xstreamOp a b)))")
	put!(env, -"filter", fromString "(forall (a) (apply map (comma (apply map (comma a) bool)) (apply xstreamOp a a)))")
	put!(env, -"pair", fromString "(forall (a b) (apply map (comma a b) (apply pair a b)))")
	put!(env, -"range", fromString "(apply map (comma int int) (apply generator int))")
	put!(env, -"toList", fromString "(forall (x) (apply map (comma) (apply finaliser x (apply list x))))")
	put!(env, -"one", fromString "int")
	put!(env, -"ten", fromString "int")
	return env;

TestFun: with
    test: () -> ()
== add
    import from Annotated(Expression, TPoss)
    import from Annotated(Expression, TypeTerm)
    import from Assert TPoss
    import from Assert TypeTerm	
    import from Compiler2
    import from TypeTerm
    
    test(): () ==
        test1()
	-- testXX() -- Not something we allow in any case
	test2()
	test3()

    test99(): () ==
        import from TestEnvs
	env := filterEnv()
	expr: Expression := fromString "(lambda (a) (apply qq (apply qq a (apply map (lambda (x) (apply pair x x)))) (apply toList)))"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline

    test1(): () ==
        import from TestEnvs
	env := filterEnv()
	expr: Expression := fromString "(apply range one ten)"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline

    testXX(): () ==
        import from TestEnvs
	env := filterEnv()
	expr: Expression := fromString "(lambda (x) (apply x x))"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline

    test2(): () ==
        import from TestEnvs
	env := filterEnv()
	expr: Expression := fromString "(apply qq (apply range one ten) (apply toList))"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline
	assertEquals(fromString "(apply list int)", annotation(tdn, expr))
	
    test3(): () ==
        import from TestEnvs
	env := filterEnv()
	expr: Expression := fromString "(apply qq (apply range one ten) (apply map (lambda (x) (apply pair x x))))"
	unit: Unit Expression := compile(env, expr)
	tdn := tdnAnnotations(unit)$TopDown2
	stdout << "tdn " << tdn << newline
	assertEquals(fromString "(apply pair int int)", annotation(tdn, expr))

#if ALDORTEST
#include "types"
#include "aldorio"
#pile

import from GeneratorFun
import from TestEnvs
--example()

--test()$TestFun

#endif

