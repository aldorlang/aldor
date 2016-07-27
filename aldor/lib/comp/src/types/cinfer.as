#include "comp.as"
#include "aldorio.as"
#pile

import from String

ConstraintType: Category == with
    activevars: % -> Set Symbol

EqConstraint: PrimitiveType with
    SExpressionType
    TypeFns %
    ConstraintType
    eq: (TType, TType) -> %
    unify: % -> Subst
== add
    Rep == Cross(TType, TType)
    import from TType
    import from Symbol
    
    local silly(x: %): (TType, TType) ==
        tmp: Cross(TType, TType) := rep x
	tmp

    eq(t1: TType, t2: TType): % == per((t1, t2)@Rep)

    unify(eq: %): Subst == unify(rep eq)

    sexpression(eq: %): SExpression ==
        (t1, t2) := silly eq
	[sexpr(-"eq"), sexpression t1, sexpression t2]

    activevars(eq: %): Set Symbol ==
        (t1, t2) := silly eq
        union(ftv t1, ftv t2)

    ftv(eq: %): Set Symbol ==
        (t1, t2) := silly(eq)
        union(ftv t1, ftv t2)

    apply(sigma: Subst, eqc: %): % ==
        import from TType
        (t1, t2) := silly(eqc)
        eq(sigma t1, sigma t2)

    (a: %) = (b: %): Boolean == error("eq")

ImplInstConstraint: PrimitiveType with
    SExpressionType
    TypeFns %
    ConstraintType
    iinst: (t1: TType, t2: TType, mvars: List TType) -> %
    t1: % -> TType
    t2: % -> TType
    mvars: % -> List TType
== add
    Rep == Record(t1: TType, t2: TType, mvars: List TType);
    import from Rep, TType, Symbol, List TType

    t1(iinst: %): TType == rep(iinst).t1
    t2(iinst: %): TType == rep(iinst).t2
    mvars(iinst: %): List TType == rep(iinst).mvars

    iinst(t1: TType, t2: TType, mvars: List TType): % == per [t1, t2, mvars]
   
    sexpression(iinst: %): SExpression ==
        [sexpr(-"iinst"), sexpression t1 iinst, sexpression t2 iinst,
   		      	  	      	  [sexpression var for var in mvars iinst]]

    activevars(iinst: %): Set Symbol ==
        import from Fold2 Set Symbol
        union(ftv t1 iinst, intersection((union,[])/(ftv type for type in mvars iinst), ftv t2 iinst))
	
    ftv(iinst: %): Set Symbol == never					  

    apply(sigma: Subst, i: %): % == iinst(sigma t1 i, sigma t2 i, [sigma t for t in mvars i])

    (a: %) = (b: %): Boolean == error("eq")

ExplInstConstraint: PrimitiveType with
    SExpressionType
    TypeFns %
    ConstraintType
    einst: (TType, Scheme) -> %
    t1: % -> TType
    t2: % -> Scheme
== add
    Rep == Record(t1: TType, t2: Scheme)
    import from Rep
    
    t1(einst: %): TType == rep(einst).t1
    t2(einst: %): Scheme == rep(einst).t2

    einst(t1: TType, t2: Scheme): % == per [t1, t2]

    sexpression(iinst: %): SExpression ==
        import from Symbol
        [sexpr(-"einst"), sexpression t1 iinst, sexpression t2 iinst]

    activevars(iinst: %): Set Symbol == union(ftv t1 iinst, ftv t2 iinst)

    ftv(iinst: %): Set Symbol == never					  

    apply(sigma: Subst, iinst: %): % == einst(sigma t1 iinst, sigma t2 iinst)

    (a: %) = (b: %): Boolean == error("eq")
    

Constraint: PrimitiveType with
    SExpressionType
    TypeFns %
    ConstraintType
    eq: (TType, TType) -> %
    iinst: (TType, TType, List TType) -> %
    einst: (TType, Scheme) -> %

    eq?: % -> Boolean
    iinst?: % -> Boolean
    einst?: % -> Boolean

    unify: % -> Subst

    eq: % -> EqConstraint
    iinst: % -> ImplInstConstraint
    einst: % -> ExplInstConstraint
== add
    Rep == Union(eq: EqConstraint, iinst: ImplInstConstraint, einst: ExplInstConstraint)
    import from Rep, EqConstraint, ImplInstConstraint
    import from OutputTypeFunctions %

    eq(t1: TType, t2: TType): % == per [eq(t1, t2)]
    iinst(t1: TType, t2: TType, mvars: List TType): % == per [iinst(t1, t2, mvars)]
    einst(t1: TType, t2: Scheme): % == per [einst(t1, t2)]

    eq?(c: %): Boolean == rep(c) case eq
    iinst?(c: %): Boolean == rep(c) case iinst
    einst?(c: %): Boolean == rep(c) case einst

    eq(c: %): EqConstraint == rep(c).eq
    iinst(c: %): ImplInstConstraint == rep(c).iinst
    einst(c: %): ExplInstConstraint == rep(c).einst

    sexpression(c: %): SExpression == select rep c in
        eq => sexpression(rep(c).eq)
        iinst => sexpression(rep(c).iinst)
        einst => sexpression(rep(c).einst)
	error("type")

    activevars(c: %): Set Symbol == select rep c in
        eq => activevars(rep(c).eq)
        iinst => activevars(rep(c).iinst)
        einst => activevars(rep(c).einst)
        error("type")
        

    ftv(c: %): Set Symbol == select rep c in
        eq => ftv rep(c).eq
        iinst => ftv rep(c).iinst
        einst => ftv rep(c).einst
	error("type")

    apply(sigma: Subst, c: %): % == select rep c in
        eq => per [apply(sigma, rep(c).eq)]
        iinst => per [sigma rep(c).iinst]
        einst => per [sigma rep(c).einst]
	error("type")

    unify(c: %): Subst ==
        eq? c => unify(rep(c).eq)
	error("No unification for " + toString c)

    (a: %) = (b: %): Boolean == error("eq")

TypeAssignment: SExpressionType with
    assign: (Exp, TType) -> %
    empty: () -> %
    union: (%, %) -> %
    apply: (%, Exp) -> TType
== add
    Rep == HashTable(TypedPointer Exp, TType)
    import from TypedPointer Exp
    import from Rep
    
    assign(e: Exp, t: TType): % == per [(pointer e, t)@Cross(TypedPointer Exp, TType)]
    empty(): % == per []

    union(s1: %, s2: %): % ==
        tbl := copy(rep s1)
	for (ptr, t) in rep s2 repeat tbl.ptr := t
	per tbl

    apply(tbl: %, e: Exp): TType == rep(tbl).(pointer e)

    sexpression(tbl: %): SExpression ==
        import from Symbol, String, TType, Exp
	[sexpr(-"types"), [[sexpression value ptr, sexpression type] for (ptr, type) in rep tbl]]

CTypeInfer: with
    fullti: (TypeEnv, Exp) -> (Subst, TypeAssignment)
    ti: (TypeEnv, Exp) -> (Subst, TType)
    solve: List Constraint -> Subst
== add
    import from State
    C ==> Cross(Symbol, TType)
    import from List Constraint, Constraint, List TType, TType, Scheme
    import from List List C
    import from Fold2(Constraint, List Constraint)
    import from Fold2(List Constraint)
    import from Fold2(List C)
    import from Fold2(C)
    import from ImplInstConstraint, ExplInstConstraint
    import from List Cross(Symbol, Exp)
    import from List Cross(List C, List Constraint, TType)
    import from Set Symbol
    import from Symbol
    import from Lit
        
    ti(env: TypeEnv, exp: Exp): (Subst, TType) ==
        import from TypeAssignment
        (sigma, full) := fullti(env, exp)
	(sigma, full.exp)
	
    fullti(env: TypeEnv, exp: Exp): (Subst, TypeAssignment) ==
        import from Set Symbol, SExpression, Symbol
	import from List C, List Constraint
        (a, c, t) := constraints([], exp)
	stdout << "Result: " << exp << newline
	stdout << "A: " << [[sexpr(v), sexpression(tt)] for (v, tt) in a] << newline
	stdout << "C: " << c << newline
	stdout << "T: " << t << newline
	extra := (append!, [])/([einst(type, s) for (sym, type) in a | sym = var] for (var, s) in env)
	stdout << "extra: " << extra << newline
	c := append!(extra, c)
	sigma := solve(c)
	(sigma, t)

    solve(c: List Constraint): Subst ==
        import from Set Symbol, Scheme
	import from Fold2 Set Symbol
        empty? c => null
	stdout << "Solve: "<< first c << newline
        eq? first c =>
	    sigma := unify(first c)
	    solve([sigma c1 for c1 in rest c]) * sigma
	iinst? first c =>
	    boundvars := (union, [])/(ftv type for type in mvars iinst first c)
	    freevars := ftv(t2 iinst first c) - boundvars
	    stdout << "C: " << c << newline
	    stdout << "bound: " << boundvars << newline
	    stdout << "free: " << freevars << newline
	    stdout << "active: " << activevars c << newline
	    stdout << "active(rest): " << activevars rest c << newline
	    stdout << "test intersect(" << freevars << "," << activevars rest c << ") = " << intersection(freevars, activevars rest c) << newline
	    stdout << "II: " << [sym for sym in freevars | member?(sym, activevars rest c)] << newline
	    if empty? intersection(freevars, activevars rest c) then
	    	newconstraint: Constraint := einst(t1 iinst first c, scheme(freevars, t2 iinst first c))
	        solve(cons(newconstraint, rest c))
	    else
		stdout << "Delayed.. " << first c << newline
	        solve(append!(rest c, [first c]))
	einst? first c =>
	    solve(cons(eq(t1 einst first c, instantiate t2 einst first c), rest c))
	error("unknown constraint")

    local activevars(l: List Constraint): Set Symbol ==
        import from Fold2 Set Symbol
        (union, [])/(activevars constraint for constraint in l)

    constraints(mvars: List TType, exp: Exp): (List C, List Constraint, TypeAssignment) ==
      import from Exp, TType, SExpression
      constraintsRec(mvars: List TType, exp: Exp): (List C, List Constraint, TType) ==
    	stdout << "(C: " << exp << newline
        (a, c, t) := constraints0(mvars, exp)
    	stdout << "(C: " << exp << newline
	stdout << "A: " << [[sexpr(v), sexpression(tt)] for (v, tt) in a] << newline
	stdout << "C: " << c << newline
	stdout << "T: " << t << newline
	stdout << " ))" << newline
	free tbl := union(tbl, assign(exp, t))
	(a, c, t)

      constraints0(mvars: List TType, exp: Exp): (List C, List Constraint, TType) == select exp in
	app =>
	    tv := newTypeVar()
	    (e1, e2) := app exp
	    (a1, c1, t1) := constraintsRec(mvars, e1)
	    (a2, c2, t2) := constraintsRec(mvars, e2)
	    (append!(a1, a2), append!(c1, append!(c2, eq(t1, typeMap(t2, tv)))), tv)
	var$'var' =>
	    tv := newTypeVar()
            ([(var(exp)$Exp, tv)@C], [], tv)
	_if =>
	    tv := newTypeVar()
            (tst, conseq, anti) := _if exp
	    (ta, tc, tt) := constraintsRec(mvars, tst)
	    (ca, cc, ct) := constraintsRec(mvars, conseq)
	    (aa, ac, at) := constraintsRec(mvars, anti)
	    ia := (append!, [])/[ta, ca, aa]
	    ic := [eq(ct, at), eq(tt, bool), eq(ct, tv), eq(at, tv)]
	    ic := append!(tc, append!(cc, append!(ac, ic)))
	    (ia, ic, tv)	    
	abs =>
	    tv := newTypeVar()
	    (lvar, body) := abs exp
	    (a1, c1, t1) := constraintsRec(cons(tv, mvars), body)
	    ([(avar, atype) for (avar, atype) in a1 | avar ~= lvar],
	     append!([eq(type, tv) for (xvar, type) in a1 | xvar = lvar], c1), typeMap(tv, t1))
	_let =>
	    (var, defexp, bodyexp) := _let exp
	    (a1, c1, t1) := constraintsRec(mvars, defexp)
	    (a2, c2, t2) := constraintsRec(mvars, bodyexp)
	    (append!(a1, [(avar, type) for (avar, type) in a2 | var ~= avar]),
	     append!([iinst(tp, t1, mvars) for (xvar, tp) in a2 | xvar = var],
	         (cons, c2)/c1),
	     t2)
	letrec =>
	    (bindings, body) := letrec exp
	    typevars: HashTable(Symbol, TType) := [(v, newTypeVar()) for (v, decl) in bind bindings]
	    vars := [v for (v, decl) in bind bindings]
	    mvars0 := append!([typevars.v for (v, decl) in bind bindings], mvars)
	    declresults := [constraintsRec(mvars0, decl) for (v, decl) in bind bindings]
	    (ba, bc, bt) := constraintsRec(mvars, body)
	    -- assumptions: ba + sum(ai) - vars
	    a := (append!, [])/([(v, t) for (v, t) in ai | not member?(v, vars)] for (ai, ci, ti) in declresults)
	    a := append!([(v, t) for (v, t) in ba | not member?(v, vars)], a)
	    -- constraints: eq(tvi, ti)
	    --              iinst(tp, ti) for each var in ba, ai
	    eqc := [eq(typevars.v, ti) for (ai, ci, ti) in declresults for (v, decl) in bind bindings]
            bab: List C := [(v, t) for (v, t) in ba | member?(v, vars)]
	    bounda := (append!, bab)/([(v, t) for (v, t) in ai | member?(v, vars)] for (ai, ci, ti) in declresults)
	    iinstc := [iinst(type, typevars.v, mvars) for (v, type) in bounda]
	    -- return type: bt
	    (a, ((append!, append!(eqc, append!(iinstc, bc)))/(dc for (da, dc, dt) in declresults)), bt)
	lit =>
	    l := lit exp
	    ([], [], if l case int then int else bool)
	error("unknown type" + toString(exp))
      tbl: TypeAssignment := empty()
      (resa, resc, rest) := constraintsRec(mvars, exp)
      (resa, resc, tbl)
      
#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

import from Assert TType
import from Assert Scheme
import from Assert Set Symbol
import from Symbol

exampleEnv(): TypeEnv ==
    import from Fold2 TypeEnv, Symbol, Scheme, TType, List TypeEnv, Symbol, Set Symbol
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

testEnv(): () ==
    import from Set Symbol, TypeEnv, TType, Scheme
    any := typeTVar(-"A")
    listAny := typeApp(typeId(-"List"), any)
    e := singleton(-"nil", scheme([-"A"], listAny))
    assertEquals([], ftv e)
testEnv()

testSchema(): () ==
    import from Scheme, Set Symbol, TType
    list := typeTVar(-"List")
    tv := typeTVar(-"A")
    scheme := scheme([-"A"], typeApp(list, tv))
    assertEquals([-"List"], ftv scheme)

testSchema()

testLet(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(let ((x 1)) x)"
    (subst, tp) := ti(exampleEnv(), prog)
    assertEquals(subst tp, int);

testLet()

testLambda(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda x (x 0))"
    (subst, tp) := ti(exampleEnv(), prog)
    tv := newTypeVar()$State
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), typeMap(typeMap(int, tv), tv)))

testLambda()

testLambda2(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda fn (letrec ((m (lambda l (if (nil? l) nil ((cons (fn (first l))) (m (rest l))))))) m))"
    (subst, tp) := ti(exampleEnv(), prog)
    tv1 := newTypeVar()$State
    tv2 := newTypeVar()$State
    list := typeId(-"List")
    assertEquals(generalise(empty(), subst tp),
                 generalise(exampleEnv(), typeMap(typeMap(tv1, tv2), typeMap(typeApp(list, tv1),
		 		     			      	      typeApp(list, tv2)))))
testLambda2()

testLetrec1(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(letrec ((x 1)) x)"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
    tv := newTypeVar()$State
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), int))
testLetrec1()

testLetrec2(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(letrec ((x (lambda a (x a)))) (x 1))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
    tv := newTypeVar()$State
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), tv))
testLetrec2()

testLambda3(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(letrec ((copy (lambda l (if (nil? l) nil ((cons (first l)) (copy (rest l))))))) copy)"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << tp << newline
    tv := newTypeVar()$State
    list := typeId(-"List")
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), typeMap(typeApp(list, tv), typeApp(list, tv))))
testLambda3()


testLetrec3(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(letrec ((x cons)) x)"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
    tv := newTypeVar()$State
    list := typeId(-"List")
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), typeMap(tv, typeMap(typeApp(list, tv), typeApp(list, tv)))))
testLetrec3()


testLetrec4(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda fn (letrec ((x (fn 2))) x))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
    tv := newTypeVar()$State
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), typeMap(typeMap(int, tv), tv)))

testLetrec4()

testLet1(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda fn (let ((x (fn 2))) x))"
    (subst, tp) := ti(exampleEnv(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
    tv := newTypeVar()$State
    assertEquals(generalise(empty(), subst tp),
                 generalise(empty(), typeMap(typeMap(int, tv), tv)))
testLet1()

#endif
#if 0
    ll : List Constraint := [eq(tv7, tv4), eq(tv7, typeMap(int, tv6)), eq(tv5, tv6), eq(tv4, tv8), iinst(tv8, tv5, [tv4])]

LETREC
[(eq (tvar %v15) (tvar %v12)),
 (eq (tvar %v15) (map (int) (tvar %v14))),
 (eq (tvar %v13) (tvar %v14)),
 (iinst (tvar %v16) (tvar %v13) ((tvar %v12)))]
T: (map (tvar %v12) (tvar %v16))

LET
[(eq (tvar %v3) (tvar %v1)),
 (iinst (tvar %v4) (tvar %v2) ((tvar %v1))),
 (eq (tvar %v3) (map (int) (tvar %v2)))]
T: (map (tvar %v1) (tvar %v4))
#endif
