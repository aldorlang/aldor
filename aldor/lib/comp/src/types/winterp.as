#include "comp"
#include "aldorio"
#pile
    
    
Value: SExpressionType with
    int: % -> Integer
    bool: % -> Boolean
    fn: % -> Value -> Value
    undefined?: % -> Boolean

    val: Integer -> %
    val: Boolean -> %
    val: (% -> %) -> %
    undefined: () -> %
== add
    Rep == Union(i: Integer, b: Boolean, fn: % -> %, undefined: Boolean)
    import from Rep
    import from Symbol, String
    
    int(v1: %): Integer == rep(v1).i
    bool(v2: %): Boolean == rep(v2).b
    fn(v: %): (% -> %) == rep(v).fn

    undefined(): % == per [undefined==true]
    undefined?(v: %): Boolean == rep(v) case undefined
    val(i: Integer): % == per [i]
    val(bool: Boolean): % == per [b==bool]
    val(fn: % -> %): % == per [fn]

    sexpression(v2: %): SExpression == select rep(v2) in
        i => sexpr int v2
        b =>
	    flg := bool v2
	    if flg then sexpr("true") else sexpr("false")
        fn => sexpr("function")
	undefined => sexpr("#undef")
	never
	
Binding: SExpressionType with
    name: % -> Symbol
    value: % -> Value

    binding: (Symbol, Value) -> %
    update!: (%, Value) -> ()
== add
    Rep == Record(name: Symbol, value: Value)
    import from Rep
    
    name(bind: %): Symbol == rep(bind).name
    value(bind: %): Value == rep(bind).value
    update!(bind: %, v: Value): () == rep(bind).value := v

    binding(name: Symbol, value: Value): % == per [name, value]

    sexpression(bind: %): SExpression ==
        import from Symbol, String
        [sexpr(-"bind"), sexpr name bind, sexpression value bind]

Bindings: SExpressionType with
    create: List Binding -> %
    lookup: (%, Symbol) -> Partial Value
    update!: (%, Symbol, Value) -> ()
== add
    Rep == HashTable(Symbol, Binding)
    import from Rep
    import from Binding
    
    create(bindings: List Binding): % ==
        import from Fold2 HashTable(Symbol, Binding)
	tbl := [(name bind, bind) for bind in bindings]
	per tbl

    lookup(bindings: %, name: Symbol): Partial Value ==
        import from Partial Binding
        stdout << "Lookup " << name << " " << bindings << newline
        bnd := find(name, rep bindings)
	if failed? bnd then failed else [value retract(bnd)]

    sexpression(bindings: %): SExpression == [sexpression bind for (name, bind) in rep(bindings)]

    update!(bindings: %, sym: Symbol, v: Value): () ==
        update!(rep(bindings).sym, v)

IEnv: with
    SExpressionType
    lookup: (%, Symbol) -> Partial Value
    root: () -> %
    extendEnv: (%, Bindings) -> %
    update!: (%, Symbol, Value) -> ()
== add
    Rep == List Bindings
    import from Rep, Bindings
    
    lookup(env: %, name: Symbol): Partial Value ==
        for bindings in rep(env) repeat
	    pval := lookup(bindings, name)
	    if not failed? pval then return pval
	stdout << "Missing " << name << newline
	failed
	
    root(): % == per []

    extendEnv(parent: %, env: Bindings): % == per cons(env, rep(parent))

    update!(env: %, sym: Symbol, val: Value): () == update!(first rep env, sym, val)

    sexpression(env: %): SExpression ==
        [sexpression bindings for bindings in rep env]


Interp: with
    eval: (IEnv, Exp) -> Value
== add
    import from IEnv, Lit, Bindings, Binding, Partial Value, List Binding
    import from String, Integer, List Cross(Symbol, Exp)
    
    eval(env: IEnv, exp: Exp): Value ==
      stdout << "Eval: " << exp << newline
      select exp in
        var => retract lookup(env, var exp)
	lit => select lit exp in
	           int => val(fromString(int lit exp)@Integer)
		   bool =>
		       l := lit exp
		       if bool l = "true" then val true else val false
		   error("unknown literal")
        abs =>
	    (varexp, bodyexp) := abs exp
	    evalAbs(env, varexp, bodyexp)
	app =>
	    (fnexp, argexp) := app exp
	    fnval := eval(env, fnexp)
	    arg := eval(env, argexp)
	    (fn fnval)(arg)
	_if =>
	    (tstexp, trueexp, falseexp) := _if exp
	    flg := eval(env, tstexp)
	    if bool flg then eval(env, trueexp) else eval(env, falseexp)
	_let =>
	    (sym, defexp, valexp) := _let exp
	    evalLet(env, sym, defexp, valexp)
	letrec =>
	    (bindings, valexp) := letrec exp
	    stdout << "Bindings: " << bindings << newline
	    newBindings: Bindings := create [binding(xsym, undefined()) for (xsym, xdefexp) in bind bindings]
	    stdout << "newbindings: " << newBindings << newline
	    newEnv := extendEnv(env, newBindings)
	    for (xsym, xdefexp) in bind bindings repeat
	        val := eval(newEnv, xdefexp)
		update!(newEnv, xsym, val)
	    eval(newEnv, valexp)
	error("Not implemented " + toString(exp))

    evalAbs(env: IEnv, symbol: Symbol, bodyExp: Exp): Value ==
        fn(argval: Value): Value ==
	        newEnv := extendEnv(env, create [binding(symbol, argval)])
	        eval(newEnv, bodyExp)
	val fn

    evalLet(env: IEnv, symbol: Symbol, defexp: Exp, valexp: Exp): Value ==
        val := eval(env, defexp)
	newEnv := extendEnv(env, create [binding(symbol, val)])
	eval(newEnv, valexp)
	

#if ALDORTEST
#include "comp"
#pile
import from String, Value, Exp, Bindings, Binding, IEnv, Integer, Interp, List Binding
import from Assert Integer, Symbol

baseEnv(): IEnv == extendEnv(root(),
			     create [binding(-"inc", val((a: Value): Value +-> val(int a + 1))),
			     	     binding(-"dec", val((a: Value): Value +-> val(int a - 1))),
			     	     binding(-"zero?", val((a: Value): Value +-> val(int a = 0)))
				     ])

testVal(): () ==
    prog: Exp := fromString "12"
    val := eval(root(), prog)
    assertEquals(12, int val)
    
testIncrement(): () ==
    prog: Exp := fromString "(inc 12)"
    val := eval(baseEnv(), prog)
    assertEquals(13, int val)

testLet(): () ==
    prog: Exp := fromString "(let ((x 5)) (inc x)))"
    val := eval(baseEnv(), prog)
    assertEquals(6, int val)

testIf(): () ==
    prog: Exp := fromString "(if true 1 0)"
    val := eval(baseEnv(), prog)
    assertEquals(1, int val)

    prog := fromString "(if false 1 0)"
    val := eval(baseEnv(), prog)
    assertEquals(0, int val)

testLambda(): () ==
    prog: Exp := fromString "(let ((f (lambda x (inc x)))) (f 1))"
    val := eval(baseEnv(), prog)
    assertEquals(2, int val)

testLetRec(): () ==
    prog: Exp := fromString "(letrec ((f (lambda x (if (zero? x) 0 (inc (inc (f (dec x)))))))) (f 3))"
    val := eval(baseEnv(), prog)
    assertEquals(6, int val)


testVal()
testIncrement()
testLet()
testIf()
testLambda()
testLetRec()

#endif
