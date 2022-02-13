#include "types"
#include "aldorio"
#pile

ListSet(T: PrimitiveType): PrimitiveType with
    if T has ExpressionType then ExpressionType
    if T has SExpressionOutputType then SExpressionOutputType
    generator: % -> Generator T
    cons: (t: T, s: %) -> %
    remove: (s: %, t: T) -> %
    bracket: Tuple T -> %
    bracket: Generator T -> %
    member?: (T, %) -> Boolean
    +: (%, %) -> %
    -: (%, %) -> %
    intersect: (%, %) -> %
    union: (%, %) -> %
    empty: () -> %
    empty?: % -> Boolean
    #: % -> MachineInteger
== List T add
    Rep == List T
    import from Rep
    import from BooleanFold
    import from T

    empty(): % == per []
    cons(t: T, s: %): % == if member?(t, s) then s else per cons(t, rep s)
    member?(t: T, s: %): Boolean == _or/(elt = t for elt in rep s)
    remove(s: %, t: T): % == per [ x for x in s | x ~= t]

    empty?(s: %): Boolean == empty? rep s

    (a: %) + (b: %): % == per append!(copy rep a, [elt for elt in b | not member?(elt, a)])
    (a: %) - (b: %): % == per [elt for elt in rep a | not member?(elt, rep b)]

    intersect(a:%, b: %): % == per [x for x in rep a | member?(x, rep b)]
    union(a:%, b: %): % == 
        u: % := a
	while not empty? b repeat
	    if not member?(first rep b, a) then u := cons(first rep b, u)
	    b := per rest rep b
	return u

    if T has ExpressionType then
        import from List ExpressionTree
        extree(s: %): ExpressionTree == ExpressionTreeList [extree e for e in rep s]

    (a: %) = (b: %): Boolean ==
        import from MachineInteger
        empty? a => empty? b
        # a = # b and _and/(member?(x, rep a) for x in rep b)

    if T has SExpressionOutputType then
        sexpression(l: %): SExpression == [sexpression t for t in l]

