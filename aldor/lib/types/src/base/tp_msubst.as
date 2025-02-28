#include "types"
#include "aldorio"
#pile

Constraint: SExpressionType with
    PrimitiveType

    expressions: % -> List Expression
    isEq: Expression -> %
    +: (%, %) -> %
== add
    Rep == List Expression

    isEq(a: Expression): % == per [a]
    (a: %) + (b: %): % == per(append(rep a, rep b))

CSub: SExpressionType with
    PrimitiveType
    
    empty: () -> %
    addSubst: (%, Symbol, Expression)

    substs: % -> List Cross(Symbol, Expression)
    constraints: % -> List Cross(Symbol, Constraint)
    cvars: % -> ListSet Symbol

    compose: (%, %) -> %
    merge: (%, %) -> %
    sigma: % -> Substitution

    empty?: % -> Boolean
    failed?: % -> Boolean
== add
    Rep == Record(sigma: Substitution,
		  constrs: List Cross(Symbol, Constraint))

    import from Rep
    
    empty(): per [ [], [], [] ]

    compose(a: %, b: %): % ==
        not canCompose?(a, b) => error "Failed to compose"
        per[compose(sigma a, sigma b), cvars a + cvars b,
                                  append([(s, b(x)) for (s, x) in rep(a).constrs], rep(b).constrs))

    -- A -> M [M={X, Y}]
    -- B -> F A []
    -- B o A ==> B -> F M [M={B.X, B.Y}]

    merge(l: %, r: %): % ==
        local mm: List Cross(Symbol, Symbol, Expression, Expression)
	local nc: List cross(Symbol, Constraint)
        (vl, vr, vs) := triage(vars l, vars r)
	mm:  := [(v, gensym(), retract lookup(v, l), retract lookup(v, r)) for v in vs]
	ll := [(v, e) for (v, e) in l | contains?(vl, v)]
	      + [(v, e) for (v, e) in l | contains?(vl, v)]
	      + [(v, k) for (v, k, e1, e2) in mm]
	nc := [(k, e1, e2) for (v, k, e1, e2) in mm]
	per [sigma mm, nc]

    local triage(a: ListSet Symbol, b: ListSet Symbol): (ListSet Symbol, ListSet Symbol, ListSet Symbol) ==
        local justA, justB, both: ListSet Symbol
        justA := []
	justB := []
	both  := []
	for x in a repeat
	    if contains?(b, x) then
	        both := [x] + both
            else
	        justA := [x] + justA
        for x in b repeat
	    if not contains?(both, x) then
	        justB := [x] + justB
	(justA, justB, both)

    -- identify cases with shared free variables
    local canCompose?(a: %, b: %): Boolean == true

-- cons:
--- Vx.(x, Lx) -> Lx
--- Vx.(x, Kx) -> (Kx, F)

-- L(a,c).f(a, b) + g(c, d)
--> infer eq(S_1), eq(S_2), etc

-- a: oneOf(M)
