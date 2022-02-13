#include "types"
#include "aldorio"
#pile

TPoss: Join(PrimitiveType, SExpressionOutputType) with
    empty: () -> %
    addOne: (%, TypeTerm, ParamSet) -> %
    find: (%, TypeTerm) -> Partial ParamSet
    bracket: Generator TypeTerm -> %
    bracket: Tuple TypeTerm -> %
    bracket: Generator Cross(TypeTerm, ParamSet) -> %
    bracket: Tuple Cross(TypeTerm, ParamSet) -> %
    generator: % -> Generator Cross(TypeTerm, ParamSet)

    #: % -> MachineInteger

    -- temp
    satisfies?: (TypeTerm, TypeTerm) -> Boolean
== add
    TPossItem == Record(ptt: TypeTerm, psub: ParamSet)
    Rep == List TPossItem
    import from Rep, TPossItem
    import from List Substitution
    import from Partial Rep
    import from UnificationTools
    import from Substitution
    import from MachineInteger
    import from BooleanFold
    import from Expression
    import from ParamSet
    
    empty(): % == per []

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
	
    satisfies?(s: TypeTerm, t: TypeTerm): Boolean ==
	constant? t => s = t
	ur := unify(constant expr s, t)
	not failed? ur

    local satisfies?(ps: TPossItem, pt: TPossItem): Boolean ==
        if empty? ps.psub and empty? pt.psub then satisfies?(ps.ptt, pt.ptt) else false

    local mergeDefs(l1: ParamSet, l2: ParamSet): ParamSet ==
        empty? l1 => l2
    	empty? l2 => l1
	never

    find(tp: %, tt: TypeTerm): Partial ParamSet ==
        empty? rep tp => failed
	candidate := first rep tp
	satisfies?(tt, candidate.ptt) => [candidate.psub]
	find(per rest rep tp, tt)

    sexpression(tp: %): SExpression ==
        import from Symbol
	subPart(tt: ParamSet): SExpression == if empty? tt then [] else [sexpression tt]
        cons(sexpr(-"tposs"), [cons(sexpression candidate.ptt, subPart candidate.psub) for candidate in rep tp])

TestTPoss: with
    testTPoss: () -> ()
== add
    import from TypeTerm
    import from Substitution
    import from List Substitution
    import from Assert TypeTerm
    import from Partial ParamSet
    import from TPoss
    import from Expression
    import from ParamSet
    
    testTPoss(): () ==
        testBasics()
	testSat()
	testQuantifiers()

    local testBasics(): () ==
        tp: TPoss := empty()
	tp := addOne(tp, fromString "int", empty())
	assertFalse(failed? find(tp, fromString "int"))
	assertTrue(failed? find(tp, fromString "wibble"))

    local testSat(): () ==
        tt1: TypeTerm := fromString "int"
	tt2: TypeTerm := fromString "(forall (x) x)"
	assertTrue(satisfies?(tt1, tt2))
	assertFalse(satisfies?(tt2, tt1))
	tt1 := fromString "(forall (x) (apply plus x one))"
	tt2 := fromString "(forall (a b) (apply plus a b))"
	assertTrue(satisfies?(tt1, tt2))
	assertFalse(satisfies?(tt2, tt1))

    local testQuantifiers(): () ==
	tp := addOne(empty(), fromString "(forall (x) x)", empty())
	assertFalse(failed? find(tp, fromString "int"))

	tp := addOne(empty(), fromString "int", empty())
	tp := addOne(tp, fromString "(forall (x) x)", empty())
	stdout << "poss " << tp << newline
	assertFalse(failed? find(tp, fromString "(forall (k) k)"))
	
#if ALDORTEST
#include "types"
#include "aldorio"
#pile
import from TestTPoss
testTPoss()
stdout << "DONE!" << newline
#endif
