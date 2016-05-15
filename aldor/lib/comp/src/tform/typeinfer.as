#include "comp"
#include "aldorio"
#pile

TiSymbolTable: with
    ids: Field HashTable(Id, SymbolTableEntry)
    add!: (SymbolTable, Id, TForm) -> ()

    lookup: (SymbolTable, Id) -> TPoss
    lookup: (SymbolTable, Id) -> List Syme

    export from HashTable(Id, SymbolTableEntry)
    export from SymbolTableEntry
    export from SymbolTable
== add
    import from SymbolTable
    import from SymbolTableEntry
    import from Partial SymbolTableEntry
    import from TPoss
    import from String
    import from Partial SymbolTableEntry
    import from HashTable(Id, SymbolTableEntry)
    import from Syme

    ids: Field HashTable(Id, SymbolTableEntry) == field("ids", table())

    add!(tbl: SymbolTable, id: Id, tf: TForm): () ==
        if empty? fields(tbl).ids then fields(tbl).ids := table()
        entOrFail := find(id, fields(tbl).ids)
	if failed? entOrFail then
	    ent := new()
	    fields(tbl).ids.id := ent
	else
	    ent := retract entOrFail
	addMeaning!(ent, newLex(id, tf, level tbl))
	-- assert that this fn did something
	if empty? lookup(tbl, id) then error("oops")

    lookup(tbl: SymbolTable, id: Id): TPoss ==
        parentTPoss := if root? tbl then empty() else lookup(parent tbl, id)
        ent := find(id, fields(tbl).ids)
	failed? ent => parentTPoss
	union(tposs retract ent, parentTPoss)

    lookup(tbl: SymbolTable, id: Id): List Syme ==
        import from TFormSatisfaction
        parentSymes: List Syme := if root? tbl then [] else lookup(parent tbl, id)
	ent := find(id, fields(tbl).ids)
	failed? ent => parentSymes
	append!(copy(symes retract ent), parentSymes)

SymbolTableEntry: with
    new: () -> %
    tposs: % -> TPoss
    symes: % -> List Syme
    addMeaning!: (%, Syme) -> ()
== add
    Rep == Record(tposs: TPoss, symes: List Syme)
    import from Rep, TPoss
    new(): % == per [empty(), []]

    tposs(ent: %): TPoss == rep(ent).tposs

    addMeaning!(ent: %, syme: Syme): () ==
        import from SymeFields
        rep(ent).symes := cons(syme, rep(ent).symes)
        add!(rep(ent).tposs, syme.type)

    symes(ent: %): List Syme == rep(ent).symes

TypeInfer: with
    bottomUp: (SymbolTable, AbSyn) -> ()
    topDown: (SymbolTable, AbSyn, TForm) -> Boolean
== add
    import from TFormSatisfaction
    import from AbSynTags
    import from TPoss
    import from List AbSyn
    import from TiSymbolTable
    import from AbState

    bupDepth: Integer := 0
    bupCount: Integer := 0
    bottomUp(stab: SymbolTable, ab: AbSyn): () ==
        free bupCount := bupCount + 1
        free bupDepth := bupDepth + 1
	myCount := bupCount
        stdout << "(Bup: " << myCount << ab << newline
	bottomUp0(stab, ab)
        stdout << " Bup: " << myCount << tposs state ab << ")" << newline
	bupDepth := bupDepth - 1

    local bottomUp0(stab: SymbolTable, ab: AbSyn): () ==
        tag ab = apply => bupApply(stab, ab)
        tag ab = comma => bupComma(stab, ab)
        tag ab = declare => bupDeclare(stab, ab)
        tag ab = id => bupId(stab, ab)
        tag ab = _define => bupGeneric(stab, ab)
	bupGeneric(stab, ab)

    bupApply(stab: SymbolTable, ab: AbSyn): () ==
        import from AbSynApply, XSubst, TFormTagMap
	import from TFormSubst
        for abn in children ab repeat
	    bottomUp(stab, abn)
        op := applyOp ab
	opPoss := tposs state op
	resultTPoss := empty()
	for tf in opPoss repeat
            sigma: XSubst := create []
	    result := satisfiesMapArgs(std(), sigma, applyArgs ab, mapArgs tf)
	    if result then add!(resultTPoss, subst(mapRets tf, sigma))
	setTPoss(state ab, resultTPoss)

    bupDeclare(stab: SymbolTable, ab: AbSyn): () ==
        bupGeneric(stab, ab)

    bupComma(stab: SymbolTable, ab: AbSyn): () ==
        import from List TPoss
        for abn in children ab repeat
	    bottomUp(stab, abn)
	setTPoss(state ab, crossProduct [tposs state abn for abn in children ab])

    bupId(stab: SymbolTable, ab: AbSyn): () ==
        import from TiSymbolTable
	import from SymbolTable
	theId := id ab
        setTPoss(state ab, lookup(stab, theId))

    bupGeneric(stab: SymbolTable, ab: AbSyn): () ==
        import from TFormTagComma
        for abn in children ab repeat bottomUp(stab, abn)
	setTPoss(state ab, [comma()])

    tdnDepth: Integer := 0
    tdnCount: Integer := 0

    topDown(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        free tdnCount := tdnCount + 1
        free tdnDepth := tdnDepth + 1
	myCount := tdnCount
        stdout << "(Tdn: " << myCount << ab << " -- " << tf << newline
	r := topDown0(stab, ab, tf)
        stdout << " Tdn: " << myCount << " -> " << state.ab << ")" << newline
        tdnDepth := tdnDepth - 1
	r

    local topDown0(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        tag ab = apply => tdnApply(stab, ab, tf)
        tag ab = comma => tdnComma(stab, ab, tf)
        tag ab = declare => tdnDeclare(stab, ab, tf)
        tag ab = id => tdnId(stab, ab, tf)
        tag ab = _define => tdnGeneric(stab, ab, tf)
	tdnGeneric(stab, ab, tf)

    local tdnComma(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        never
    local tdnDeclare(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        never
    local tdnGeneric(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        never

    local tdnId(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        import from SymeFields, Syme
        symes: List Syme := lookup(stab, id ab)
	candidates: List Syme := []
	stdout << "id: " << ab << " " << symes << newline
	for syme in symes repeat
	    res := xsatisfies(syme.type, tf)
	    if res then candidates := cons(syme, candidates)
	empty? candidates => false
	not empty? rest candidates => false
	ab.syme := first candidates
	setFinal(state.ab, syme(ab).type)
	true

    local zip(g1: IntegerSegment MachineInteger, g2: List AbSyn, g3: List TForm): Generator Cross(MachineInteger,AbSyn,TForm) == (v1, v2, v3) for v1 in g1 for v2 in g2 for v3 in g3
        

    local xsatisfies(S: TForm, T: TForm): SatResult ==
	stdout << "(Sat: " << S << " -- " << T << newline
	r := satisfies(std(), S, T)
	stdout << " Sat: " << r << ")" << newline
	r

    local tdnApply(stab: SymbolTable, ab: AbSyn, tf: TForm): Boolean ==
        import from AbSynApply, MachineInteger, TFormTagMap, Partial TForm
	import from TFormSubst
	import from MachineInteger
	stdout << "X: " << ab << newline
	opTPoss := tposs(state(applyOp ab))
	stdout << "L: " << opTPoss << newline
	resultTPoss := empty()
	retTPoss := empty()
	for opTf in opTPoss repeat
            sigma: XSubst := create []
	    result := satisfiesMapArgs(std(), sigma, applyArgs ab, mapArgs opTf)
	    stdout << opTf << " " << result << "  " << sigma << newline
	    retTf: TForm := subst(mapRets opTf, sigma)
	    if result and xsatisfies(retTf, tf) then
	        add!(resultTPoss, opTf)
		add!(retTPoss, retTf)
	size resultTPoss ~= 1 => false
	opTf := retract unique resultTPoss
	res: Boolean := topDown(stab, applyOp ab, opTf)
	for (i, arg, argTf) in zip(1.., applyArgs ab, args mapArgs opTf) repeat
	    res := res and topDown(stab, arg, argTf)
	if res then
		setFinal(state.ab, retract unique retTPoss)
	res


	        

--------------------

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

import from SymbolTable
import from SymbolTableBinder
import from TiSymbolTable
import from Assert TForm
import from TPoss
import from TypeInfer

string(s: Literal): AbSyn ==
    import from Id
    var string s

string(s: Literal): Id == id string s

s(x: String): String == x

test(): () ==
    import from List AbSyn, AbSyn
    import from AbState
    absyn := sequence(apply("f", "a"))

    rootSymbolTable: SymbolTable := root()

    stab := bind(rootSymbolTable, absyn)

    bottomUp(stab, absyn)
    assertTrue empty? tposs state first(children absyn)

test2(): () ==
    import from TFormTagComma, TFormTagMap, TFormTagId
    import from Assert MachineInteger
    import from Assert TForm
    import from TForm
    import from Partial TForm, TFormTagComma
    import from AbState, AbSyn

    rootSymbolTable: SymbolTable := root()

    add!(rootSymbolTable, "f", map(comma(id "Int"), comma(id "String")))
    add!(rootSymbolTable, "g", map(comma(id "Int", id "Int"), comma(id "String")))
    add!(rootSymbolTable, "a", id "Int")

    ab: AbSyn := apply("f", "a")
    stab := bind(rootSymbolTable, ab)

    bottomUp(stab, ab)
    assertTrue(unique? state ab)
    tf: TForm := unique state ab
    assertTrue(comma? tf)
    assertEquals(comma(id "String"), tf)

    ab := apply("g", "a", "a")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertTrue(unique? state.ab)
    tf: TForm := unique state.ab
    assertTrue(comma? tf)
    assertEquals(comma(id "String"), tf)

    ab := apply("f")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertTrue empty? tposs state.ab

    ab := apply("f", "a", "a")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertTrue empty? tposs state.ab

--test()
--test2()

test3(): () ==
    import from TFormTagComma, TFormTagMap, TFormTagId, TFormTagApply, TFormTagDeclare
    import from Assert MachineInteger
    import from Assert TForm
    import from TForm, List TForm
    import from Partial TForm, TFormTagComma
    import from AbState, AbSyn

    rootSymbolTable: SymbolTable := root()
    -- f: (x: Int, F x) -> G x
    -- a: Int, k: F a
    add!(rootSymbolTable, "f", map(comma(declare(id "x", id "Int"), apply(id "F", [id "x"])),
                                   comma(apply(id "G", [id "x"]))))
    add!(rootSymbolTable, "a", id "Int")
    add!(rootSymbolTable, "k", apply(id "F", [id "a"]))

    stdout << s "RRR " << rootSymbolTable << newline
    ab: AbSyn := apply("f", "a", "k")

    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)

    assertTrue(unique? state ab)
    tf: TForm := unique state ab
    assertTrue(comma? tf)
    stdout << "FIXME: TEST BROKEN!!!"@String << newline
    --assertEquals(comma(apply(id "G", [id "a"])), tf)

    topDown(stab, ab, tf)

--test3()

#endif

