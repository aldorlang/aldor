#include "comp.as"
#include "aldorio.as"
#pile

Exp: PrimitiveType with
    SExpressionType
    InputType

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
        sym? sx =>
	    sym sx = -"true" or sym sx = -"false" => per [litBoolean name sym sx]
	    per [sym sx]
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

    (<<)(rdr: TextReader): % ==
        sx: SExpression := << rdr
	parse sx

Lit: PrimitiveType with
    SExpressionType
    case: (%, 'int') -> Boolean
    case: (%, 'bool') -> Boolean
    int: % -> String
    bool: % -> String

    litBoolean: String -> %
    litInteger: String -> %
    export from 'int', 'bool'
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
