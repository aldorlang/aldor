#include "types"
#include "aldorio"
#pile

TPoss: Join(PrimitiveType, SExpressionOutputType) with
    empty?: % -> Boolean
    unique?: % -> Boolean
    unique: % -> ParamTypeTerm
    empty: () -> %
    findFirst: (%, TypeTerm) -> Partial ParamTypeTerm
    findFirst: (%, TypeTerm, (TypeTerm, TypeTerm) -> SatResult) -> Partial ParamTypeTerm
    find: (%, TypeTerm, (TypeTerm, TypeTerm) -> SatResult) -> %
    bracket: Generator ParamTypeTerm -> %
    bracket: Generator TypeTerm -> %
    bracket: Tuple TypeTerm -> %
    bracket: Generator ParamTypeTerm -> %
    generator: % -> Generator ParamTypeTerm

    #: % -> MachineInteger

== add
    TPossItem == ParamTypeTerm
    Rep == List TPossItem
    import from Rep, TPossItem
    import from List Substitution
    import from Partial Rep
    import from UnificationTools2
    import from TypeSatisfier
    import from SatResult
    import from Substitution
    import from MachineInteger
    import from BooleanFold
    import from Expression
    import from ParamSet
    import from Unifier
    
    empty(): % == per []
    empty?(tp: %): Boolean == empty? rep tp
    unique?(tp: %): Boolean == not empty? tp and empty? rest rep tp

    unique(tp: %): ParamTypeTerm ==
        not unique? tp => error "not unique"
        (first rep tp)

    bracket(g: Generator TypeTerm): % ==
        elt(tt: TypeTerm): TPossItem == asParam tt
        per [ elt term for term in g]

    bracket(g: Generator ParamTypeTerm): % == per [x for x in g]

    generator(tp: %): Generator(ParamTypeTerm) == generator rep tp
    bracket(tt: Tuple(TypeTerm)): % == [element(tt, n) for n in 1..length tt]

    find(tp: %, tt: TypeTerm, sat?: (TypeTerm, TypeTerm) -> SatResult): % ==
        l0: List Cross(ParamTypeTerm, SatResult) := [(tt1, sat?(typeTerm tt1, tt)) for tt1 in tp]
        [((left unifier sr) ptt) for (ptt, sr) in l0 | sr]

    findFirst(tp: %, tt: TypeTerm, sat?: (TypeTerm, TypeTerm) -> SatResult): Partial(ParamTypeTerm) ==
        for ptt in tp repeat
	    sr := sat?(typeTerm ptt, tt)
	    sr => return [(left unifier sr) ptt]
	failed

    findFirst(tp: %, tt: TypeTerm): Partial(ParamTypeTerm) ==
        sat?(t1: TypeTerm, t2: TypeTerm): SatResult == satisfies?(t1, t2)
        findFirst(tp, tt, sat?)

    #(tp: %): MachineInteger == #(rep tp)

    (a: %) = (b: %): Boolean ==
        # a = # b
	and _and/(aItem = bItem for aItem in rep a for bItem in rep b) -- Assumes same order. Probably dodgy
    
    sexpression(tp: %): SExpression == [sexpression t for t in rep tp]

TestTPoss: with
    testTPoss: () -> ()
== add
    import from Assert MachineInteger
    import from Assert TypeTerm
    import from Expression
    import from List Substitution
    import from MachineInteger
    import from ParamSet
    import from Partial ParamTypeTerm
    import from TPoss
    import from Substitution
    import from TypeTerm
    
    testTPoss(): () ==
        testBasics()
	-- testQuantifiers() -- Disabled.  Constructor isn't absorbing ([Vx.x, int] should be equal to [Vx.x])

    local testBasics(): () ==
	tp: TPoss := [fromString("int")]
	assertFalse(failed? findFirst(tp, fromString "int"))
	assertTrue(failed? findFirst(tp, fromString "wibble"))

    local testQuantifiers(): () ==
	tp: TPoss := [fromString "(forall (x) x)"]
	assertFalse(failed? findFirst(tp, fromString "int"))
	assertEquals(1, #tp)

	tp := [fromString "int"]
	tp := [fromString "(forall (x) x)"]
	assertEquals(1, #tp)
	stdout << "poss " << tp << newline
	assertFalse(failed? findFirst(tp, fromString "(forall (k) k)"))
	stdout << "int + Vx.x --> " << tp << newline
	
	stdout << "Vx.x + int " << newline
	tp := [fromString "(forall (x) x)"]
	tp := [fromString "int", fromString "(forall (x) x)"]
	assertEquals(1, #tp)
	stdout << "Vx.x + int --> " << tp << newline
	
#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestTPoss
testTPoss()
stdout << "DONE!" << newline
#endif
