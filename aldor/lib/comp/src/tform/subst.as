#include "comp.as"
#pile

XSubst: OutputType with
    create: (Id, AbSyn) -> %
    create: (List Cross(Id, AbSyn)) -> %
    add!: (%, List Cross(Id, AbSyn)) -> ()
    lookup: (%, Id, AbSyn) -> AbSyn
    find: (%, Id) -> Partial AbSyn
    empty?: % -> Boolean
    fields: % -> DepTable
--    symbolTable: % -> SymbolTable
    export from List Cross(Id, AbSyn)
== add
    Rep == Record(fields: DepTable)

    import from Rep
    import from HashTable(Id, AbSyn)
    import from Field HashTable(Id, AbSyn)
    import from String

    absynForId: Field HashTable(Id, AbSyn) := field("absynForId")

    fields(sigma: %): DepTable == rep(sigma).fields
    empty?(sigma: %): Boolean == empty?(rep(sigma).fields.absynForId)

--    symbolTable(sigma: %): SymbolTable == rep(sigma).stab

    local initialMap(initialAbSynForId: HashTable(Id, AbSyn)): % ==
        fields: DepTable := table()
	fields.absynForId := initialAbSynForId
	per [fields]

    create(l: List Cross(Id, AbSyn)): % ==
        import from Id
	for (id, ab) in l repeat
	    if string id = "" then error("not an id")
        initialMap([pair for pair in l])
    create(id: Id, ab: AbSyn): % == initialMap([(id, ab)@Cross(Id, AbSyn)])

    add!(sigma: %, l: List Cross(Id, AbSyn)): () ==
        import from Partial AbSyn
        for (id, ab) in l repeat
	    not failed? find(id, rep(sigma).fields.absynForId) => error "Defined the same subst twice"
	    rep(sigma).fields.absynForId.id := ab

    find(sigma: %, id: Id): Partial AbSyn == find(id, rep(sigma).fields.absynForId)

    lookup(sigma: %, id: Id, alt: AbSyn): AbSyn ==
        import from Partial AbSyn
        x := find(id, rep(sigma).fields.absynForId)
	failed? x => alt
	retract x

    (o: TextWriter) << (sigma: %): TextWriter ==
        import from Id, AbSyn, String
        sep := ""
	o << "{"
        for (k, v) in rep(sigma).fields.absynForId repeat
	    o << sep << k << " --> " << v
	    sep := ","
        o << "}"
