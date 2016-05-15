#include "comp"
#pile
    
    
Value: SExpressionType with
    int: % -> Integer
    bool: % -> Boolean
    fn: % -> Value -> Value

    val: Integer -> %
    val: Boolean -> %
    val: (% -> %) -> %
== add
    Rep == Union(i: Integer, b: Boolean, fn: % -> %)
    import from Rep
    import from Symbol, String
    
    int(v1: %): Integer == rep(v1).i
    bool(v2: %): Boolean == rep(v2).b
    fn(v: %): (% -> %) == rep(v).fn

    val(i: Integer): % == per [i]
    val(bool: Boolean): % == per [bool]
    val(fn: % -> %): % == per [fn]

    sexpression(v2: %): SExpression == select rep(v2) in
        i => sexpr int v2
        b =>
	    flg := bool v2
	    if flg then sexpr("true") else sexpr("false")
        fn => sexpr("function")
	never
	
Binding: SExpressionType with
    name: % -> Symbol
    value: % -> Value

    binding: (Symbol, Value) -> %
== add
    Rep == Record(name: Symbol, value: Value)
    import from Rep
    
    name(bind: %): Symbol == rep(bind).name
    value(bind: %): Value == rep(bind).value

    binding(name: Symbol, value: Value): % == per [name, value]

    sexpression(bind: %): SExpression ==
        import from Symbol, String
        [sexpr(-"bind"), sexpr name bind, sexpression value bind]

Bindings: SExpressionType with
    create: List Binding -> %
    lookup: (%, Symbol) -> Partial Value
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
        bnd := find(name, rep bindings)
	if failed? bnd then failed else [value retract(bnd)]

    sexpression(bindings: %): SExpression == [sexpression bind for (name, bind) in rep(bindings)]

IEnv: with
    lookup: (%, Symbol) -> Partial Value
    root: () -> %
    newEnv: (%, Bindings) -> %
== add
    Rep == List Bindings
    import from Rep, Bindings
    
    lookup(env: %, name: Symbol): Partial Value ==
        for bindings in rep(env) repeat
	    pval := lookup(bindings, name)
	    if not failed? pval then return pval
	failed
	
    root(): % == per []

    newEnv(parent: %, env: Bindings): % == per cons(env, rep(parent))


