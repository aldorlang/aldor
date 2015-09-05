#include "comp"
#include "aldorio"
#pile

TiSymbolTable: with
    ids: Field HashTable(Id, SymbolTableEntry)
    add!: (SymbolTable, Id, TForm) -> ()

    lookup: (SymbolTable, Id) -> TPoss
    export from StabField HashTable(Id, SymbolTableEntry)
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

    ids: Field HashTable(Id, SymbolTableEntry) == field("ids", table())

    add!(tbl: SymbolTable, id: Id, tf: TForm): () ==
        if empty? fields(tbl).ids then fields(tbl).ids := table()
        entOrFail := find(id, fields(tbl).ids)
	if failed? entOrFail then
	    ent := new()
	    fields(tbl).ids.id := ent
	else
	    ent := retract entOrFail
	addMeaning!(ent, tf)
	-- assert that this fn did something
	if empty? lookup(tbl, id) then error("oops")

    lookup(tbl: SymbolTable, id: Id): TPoss ==
        parentTPoss := if root? tbl then empty() else lookup(parent tbl, id)
        ent := find(id, fields(tbl).ids)
	failed? ent => parentTPoss
	union(tposs retract ent, parentTPoss)

SymbolTableEntry: with
    new: () -> %
    tposs: % -> TPoss
    addMeaning!: (%, TForm) -> ()
== add
    Rep == Record(tposs: TPoss)
    import from Rep, TPoss
    new(): % == per [empty()]

    tposs(ent: %): TPoss == rep(ent).tposs
    addMeaning!(ent: %, type: TForm): () ==
        add!(rep(ent).tposs, type)

TypeInfer: with
    bottomUp: (SymbolTable, AbSyn) -> ()
== add
    import from TFormSatisfaction
    import from AbSynTags
    import from TPoss
    import from List AbSyn
    import from TiAbSynFields
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
        stdout << " Bup: " << myCount << tposs ab.state << ")" << newline
	bupDepth := bupDepth - 1

    local bottomUp0(stab: SymbolTable, ab: AbSyn): () ==
        if not field?(ab, state) then ab.state := abstate()
        tag ab = apply => bupApply(stab, ab)
        tag ab = comma => bupComma(stab, ab)
        tag ab = declare => bupDeclare(stab, ab)
        tag ab = id => bupId(stab, ab)
        tag ab = _define => bupGeneric(stab, ab)
	bupGeneric(stab, ab)

    bupApply(stab: SymbolTable, ab: AbSyn): () ==
        import from AbSynApply, Subst, TFormTagMap
	import from TFormSubst
        for abn in children ab repeat
	    bottomUp(stab, abn)
        op := applyOp ab
	abArgs := applyArgs ab
	opPoss := tposs op.state
	resultTPoss := empty()
	for tf in opPoss repeat
            sigma: Subst := create []
	    result := satisfiesMapArgs(std(), sigma, abArgs, mapArgs tf)
	    if result then add!(resultTPoss, subst(mapRets tf, sigma))
	setTPoss(ab.state, resultTPoss)

    bupDeclare(stab: SymbolTable, ab: AbSyn): () ==
        bupGeneric(stab, ab)

    bupComma(stab: SymbolTable, ab: AbSyn): () ==
        import from List TPoss
        for abn in children ab repeat
	    bottomUp(stab, abn)
	setTPoss(ab.state, crossProduct [tposs abn.state for abn in children ab])

    bupId(stab: SymbolTable, ab: AbSyn): () ==
        import from TiSymbolTable
	import from SymbolTable
	theId := ab.id
        setTPoss(ab.state, lookup(stab, theId))

    bupGeneric(stab: SymbolTable, ab: AbSyn): () ==
        import from TFormTagComma
        for abn in children ab repeat bottomUp(stab, abn)
	setTPoss(ab.state, [comma()])

--------------------

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

import from AbSyn, SymbolTable
import from SymbolTableBinder
import from TiSymbolTable
import from Assert TForm
import from TiAbSynFields
import from TPoss
import from TypeInfer

string(s: Literal): AbSyn ==
    import from Id
    id string s

string(s: Literal): Id == id string s

s(x: String): String == x

test(): () ==
    import from List AbSyn
    absyn := sequence(apply("f", "a"))

    rootSymbolTable: SymbolTable := root()

    stab := bind(rootSymbolTable, absyn)

    bottomUp(stab, absyn)
    assertTrue empty? first(children absyn).tposs

test2(): () ==
    import from TFormTagComma, TFormTagMap, TFormTagId
    import from Assert MachineInteger
    import from Assert TForm
    import from TForm
    import from Partial TForm, TFormTagComma

    rootSymbolTable: SymbolTable := root()

    add!(rootSymbolTable, "f", map(comma(id "Int"), comma(id "String")))
    add!(rootSymbolTable, "g", map(comma(id "Int", id "Int"), comma(id "String")))
    add!(rootSymbolTable, "a", id "Int")

    ab: AbSyn := apply("f", "a")
    stab := bind(rootSymbolTable, ab)

    bottomUp(stab, ab)
    assertFalse(failed? unique ab.tposs)
    tf: TForm := retract unique ab.tposs
    assertTrue(comma? tf)
    assertEquals(comma(id "String"), tf)

    ab := apply("g", "a", "a")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertFalse(failed? unique ab.tposs)
    tf: TForm := retract unique ab.tposs
    assertTrue(comma? tf)
    assertEquals(comma(id "String"), tf)

    ab := apply("f")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertTrue empty? ab.tposs

    ab := apply("f", "a", "a")
    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)
    assertTrue empty? ab.tposs

test()
test2()

test3(): () ==
    import from TFormTagComma, TFormTagMap, TFormTagId, TFormTagApply, TFormTagDeclare
    import from Assert MachineInteger
    import from Assert TForm
    import from TForm, List TForm
    import from Partial TForm, TFormTagComma

    rootSymbolTable: SymbolTable := root()

    add!(rootSymbolTable, "f", map(comma(declare("x", id "Int"), apply(id "F", [id "x"])),
                                   comma(apply(id "G", [id "x"]))))
    add!(rootSymbolTable, "a", id "Int")
    add!(rootSymbolTable, "k", apply(id "F", [id "a"]))

    stdout << s "RRR " << rootSymbolTable << newline
    ab: AbSyn := apply("f", "a", "k")

    stab := bind(rootSymbolTable, ab)
    bottomUp(stab, ab)

    assertFalse(failed? unique ab.tposs)
    tf: TForm := retract unique ab.tposs
    assertTrue(comma? tf)
    assertEquals(comma(apply(id "G", [id "a"])), tf)

test3()

#endif
