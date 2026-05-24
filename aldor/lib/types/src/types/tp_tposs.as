#include "types"
#include "aldorio"
#pile

TPossOld: Join(PrimitiveType, SExpressionOutputType) with
    empty?: % -> Boolean
    unique?: % -> Boolean
    unique: % -> TypeTerm
    empty: () -> %
    addOne: (%, TypeTerm, ParamSet) -> %
    findFirst: (%, TypeTerm) -> Partial ParamSet
    findFirst: (%, TypeTerm, (TypeTerm, TypeTerm) -> SatResult) -> Partial ParamSet
    find: (%, TypeTerm, (TypeTerm, TypeTerm) -> SatResult) -> %
    bracket: Generator TypeTerm -> %
    bracket: Tuple TypeTerm -> %
    bracket: Generator Cross(TypeTerm, ParamSet) -> %
    bracket: Tuple Cross(TypeTerm, ParamSet) -> %
    generator: % -> Generator Cross(TypeTerm, ParamSet)

    #: % -> MachineInteger

== add
    TPossItem == Record(ptt: TypeTerm, psub: ParamSet)
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

    unique(tp: %): TypeTerm ==
        not unique? tp => error "not unique"
        (first rep tp).ptt

    bracket(g: Generator TypeTerm): % ==
        elt(tt: TypeTerm): TPossItem ==
	    if param? expr tt then
	        [forall(vars tt, paramExpr expr tt), param(paramVar expr tt, tt)]
	    else
	        [tt, empty()]
        per [ elt term for term in g]

    bracket(g: Generator Cross(TypeTerm, ParamSet)): % ==
        per [ [cx] for cx in g]

    bracket(tt: Tuple TypeTerm): % == per [ [element(tt, n), empty()] for n in 1..length tt]
    bracket(tt: Tuple Cross(TypeTerm, ParamSet)): % == per [ [element(tt, n)] for n in 1..length tt]
    
    generator(tp: %): Generator Cross(TypeTerm, ParamSet) ==
        (item.ptt, item.psub) for item in rep(tp)

    #(tp: %): MachineInteger == #(rep tp)

    (a: %) = (b: %): Boolean ==
        # a = #b
	  and _and/(aItem.ptt = bItem.ptt for aItem in rep a for bItem in rep b)
	  and _and/(aItem.psub = bItem.psub for aItem in rep a for bItem in rep b)
    
    addOne(tp: %, tt: TypeTerm, tts: ParamSet): % ==
       aux(ll: Rep): Partial Rep ==
	   stdout << "Aux " << #ll << newline
           empty? ll => failed
           candidate := first ll
           if satisfies?([tt, tts], candidate) then
	       stdout << "Inc Sat C " << tt << candidate.ptt << newline
               newTop: TPossItem := [candidate.ptt, mergeDefs(tts, candidate.psub)]
	       [cons(newTop, rest ll)]
           else if satisfies?(candidate, [tt, tts]) then
	       stdout << "C Sat Inc " << candidate.ptt << tt << newline
               newTop: TPossItem := [tt, mergeDefs(tts, candidate.psub)]
	       [cons(newTop, rest ll)]
           else
	       maybe := aux(rest ll)
               if failed? maybe then failed else [cons(candidate, retract maybe)]
       if empty? rep tp then return per [[tt, tts]]
       merged := aux(rep tp)
       not failed? merged => per retract merged
       per cons([tt, tts], rep tp)
	
    local satisfies?(ps: TPossItem, pt: TPossItem): Boolean ==
        if empty? ps.psub and empty? pt.psub then test satisfies?(ps.ptt, pt.ptt) else false

    local mergeDefs(l1: ParamSet, l2: ParamSet): ParamSet ==
        empty? l1 => l2
    	empty? l2 => l1
	never

    findFirst(tp: %, tt: TypeTerm): Partial ParamSet ==
        local sat(a: TypeTerm, b: TypeTerm): SatResult == satisfies?(a, b)
        findFirst(tp, tt, sat)

    findFirst(tp: %, tt: TypeTerm, sat?: (TypeTerm, TypeTerm) -> SatResult): Partial ParamSet ==
        empty? rep tp => failed
	candidate := first rep tp
	sat?(candidate.ptt, tt) => [candidate.psub]
	findFirst(per rest rep tp, tt, sat?)

    find(tp: %, tt: TypeTerm, sat?: (TypeTerm, TypeTerm) -> SatResult): % ==
        l0: List Cross(TypeTerm, ParamSet, SatResult) := [(tt1, ps1, sat?(tt1, tt)) for (tt1, ps1) in tp]
	return [ ((left unifier sr) tt1, (left unifier sr) ps1) for (tt1, ps1, sr) in l0 | sr]

    sexpression(tp: %): SExpression ==
        import from Symbol
	subPart(tt: ParamSet): SExpression == if empty? tt then [] else [sexpression tt]
        cons(sexpr(-"tposs"), [cons(sexpression candidate.ptt, subPart candidate.psub) for candidate in rep tp])

TestTPoss: with
    testTPoss: () -> ()
== add
    import from Assert MachineInteger
    import from Assert TypeTerm
    import from Expression
    import from List Substitution
    import from MachineInteger
    import from ParamSet
    import from Partial ParamSet
    import from TPossOld
    import from Substitution
    import from TypeTerm
    
    testTPoss(): () ==
        testBasics()
	testQuantifiers()

    local testBasics(): () ==
        tp: TPossOld := empty()
	tp := addOne(tp, fromString "int", empty())
	assertFalse(failed? findFirst(tp, fromString "int"))
	assertTrue(failed? findFirst(tp, fromString "wibble"))

    local testQuantifiers(): () ==
	tp := addOne(empty(), fromString "(forall (x) x)", empty())
	assertFalse(failed? findFirst(tp, fromString "int"))
	assertEquals(1, #tp)

	tp := addOne(empty(), fromString "int", empty())
	tp := addOne(tp, fromString "(forall (x) x)", empty())
	assertEquals(1, #tp)
	stdout << "poss " << tp << newline
	assertFalse(failed? findFirst(tp, fromString "(forall (k) k)"))
	stdout << "int + Vx.x --> " << tp << newline
	
	stdout << "Vx.x + int " << newline
	tp := addOne(empty(), fromString "(forall (x) x)", empty())
	tp := addOne(tp, fromString "int", empty())
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
