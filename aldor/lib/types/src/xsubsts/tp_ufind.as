#include "types"
#include "aldorio"
#pile
import from Pointer

UFind(T: HashType, U: with): with
    if T has SExpressionOutputType then SExpressionOutputType
    new: () -> %
    empty?: % -> Boolean
--    union!: (%, T, T) -> (T, T, Partial U)
    find: (%, T) -> T
    copy: % -> %
    data: (%, T) -> Partial U
    setData!: (%, T, U) -> ()
    parents: % -> Generator Cross(T, T)
    data: % -> Generator Cross(T, U)
    export from Partial U
== add
    Rec == Record(count: MachineInteger, parent: T, data: Pointer)
    Rep == HashTable(T, Rec)
    import from Rep, Rec, Partial Rec, T, U, AldorInteger
    
    data(u: %): Generator Cross(T, U) == generate
        for (k, v) in rep u repeat
	    if not nil? v.data then
	        yield (k, rtp v.data)

    copy(u: %): % ==
        per [(t, copy v) for (t, v) in rep u]

    local copy(r: Rec): Rec == [r.count, r.parent, r.data]

    empty?(u: %): Boolean == empty? rep u

    new(): % == per table()

    local ptr(u: U): Pointer == u pretend Pointer
    local rtp(p: Pointer): U == p pretend U;

    union!(u: %, t1: T, t2: T): (base: T, other: T, otherData: Partial U) ==
        stdout << "U " << t1 << " " << t2 << newline
        init!(u, t1)
        init!(u, t2)
        rt1 := base(u, t1)
        rt2 := base(u, t2)
        stdout << "U " << t1 << " " << t2 << " " << rt1 << " " << rt2 << newline
	rt1 = rt2 =>
	    (t1, t2, failed)
	v1: Rec := rep(u).(rt1)
	v2: Rec := rep(u).(rt2)
	r: Pointer := nil
	if v1.count < v2.count then
	    (v1, v2) := (v2, v1)
	    (rt1, rt2) := (rt2, rt1)
        r := v1.data
	v2.count := 1 + v1.count + v2.count
	v1.parent := rt2
	v1.data := nil
	return (rt1, rt2, if nil? r then failed else [rtp r])

    local init!(u: %, t: T): () ==
	rep(u).t := [0, t, nil]
	
    find(u: %, t: T): T ==
        base(u, t)

    data(u: %, t: T): Partial U ==
        failed? find(t, rep(u)) => failed
        rt := base(u, t)
	p := (rep u).(rt).data
	nil? p => failed
	[rtp p]

    setData!(u: %, t: T, d: U): () ==
        failed? find(t, rep(u)) =>
	    rep(u).(t) := [0, t, ptr d]
        rt := base(u, t)
	rep(u).(rt).data := ptr d
	
    local base(u: %, t: T): T ==
        f: Partial Rec := find(t, rep(u))
	failed? f => t
	p := ((retract f).parent)
	p = t => t
	base(u, p)

    parents(u: %): Generator Cross(T, T) == generate
        for k in keys rep u repeat
	    b := base(u, k)
	    if b ~= k then yield (k, b)

    if T has SExpressionOutputType then
        import from Symbol
        sexpression(u: %): SExpression == [sexpr(-"ufind"), [cons(sexpression k, sexpression b) for (k, b) in parents u]]

#if 0
Unifier: with
    unify: (XSubstitution, Expression, Expression) -> ()
== add
    unify(xs: XSubstitution, e1: Expression, e2: Expression): Partial XSubstitution ==
        symbol? e2 and not symbol? e1 => unify(xs, e2, e1)
	symbol? e1 and not symbol? e2 =>
	    subst!(xs, sym e1, e2)
	comma? e1 and not comma? e2 => failed()
	comma? e1 and comma? e2 =>
	    # commaParts e1 ~= # commaParts e2 => failed()
	    unifyLists(xs, commaParts e1, commaParts e2)
test(): () ==
    symTab: SymbolTable := new()
    -- A, B, C are types
    -- ...
    subst: XSubstitution := empty()
    add!(subst, +"X", "string")
    add!(subst, +"Y", "int")
    add!(subst, +"Z", "(mapsTo X Y)
    subst(subst, +"Z") -- equals (mapsTo string int)
#endif


-- foo(v: V)(x: M): R == x v
-- x --> Map(Q1, Q2)
-- v --> Q1
-- R --> Q2
-- T --> Map(V, Map(M, R))

-- foo x == if x = 0 then 1 else foo(x-1)

#if ALDORTEST
#include "types"
#include "aldorio"
#pile

testUF(): () ==
    import from Symbol, String
    uf: UFind(Symbol, String) := new()
    local pair!(s1: Symbol, s2: Symbol): () == 
        (s1, s2, d) := union!(uf, s1, s2)
--        setData!(uf, s1, new(id s2 + txt d + txt data s1))
    pair!(-"x", -"y")
    pair!(-"a", -"b")
    stdout << "hello2 " << find(uf, -"x") << newline
    stdout << "hello " << find(uf, -"y") << newline
--    not(data(uf, +"x") = data(uf, +"y")) => never
--    not(data(uf, +"a") = data(uf, +"b")) => never
    return

testUF()
#endif