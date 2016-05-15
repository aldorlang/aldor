#include "comp"
#include "aldorio"
#pile

SymbolTableBinder: with
    bind: (SymbolTable, AbSyn) -> SymbolTable
== add
    import from List AbSyn
    import from SymbolTable
    default ab: AbSyn
    default tbl: SymbolTable
    -- Several passes;
    -- 1. Identify scopes
    -- 2. find definitions & declarations in each scope
    -- 3. Conditional information at each scope level
    bind(root: SymbolTable, whole: AbSyn): SymbolTable ==
        tbl := new(root, file)
	stdout << "hello" << newline
	addTables(whole, tbl)
	bindDefinitionsAndDeclarations(whole, tbl)
	bindImports(whole, tbl)
	bindConditions(whole, tbl)
	tbl

    addTables(ab, tbl): () ==
        stdout << "(Bind " << name tag ab << newline
	select tag ab in
            lambda => tbl := addTables(ab, tbl, lambda)
            _for => tbl := addTables(ab, tbl, _for)
            _if => tbl := addTables(ab, tbl, _if)
            comma => tbl := addTables(ab, tbl, comma)
	for abn in children ab repeat addTables(abn, tbl)
        stdout << " Bind)" << newline

    addTables(ab: AbSyn, tbl: SymbolTable, tag: SymbolTableType): SymbolTable ==
        ab.symbolTable := new(tbl, tag)
	for abn in children ab repeat addTables(abn, symbolTable(abn, tbl))
	symbolTable(ab, tbl)

    bindDefinitionsAndDeclarations(ab: AbSyn, tbl: SymbolTable): () ==
        select tag ab in
	    _define => bindDefine(ab, tbl)
	    declare => bindDefine(ab, tbl)
	for abn in children ab repeat
	    bindDefinitionsAndDeclarations(abn, symbolTable(abn, tbl))

    bindImports(ab: AbSyn, tbl: SymbolTable): () ==
        select tag ab in
	    _import => bindImport(ab, tbl)
	    ab -- just a null statement
	for abn in children ab repeat
	    bindImports(abn, symbolTable(abn, tbl))

    bindDefine(ab: AbSyn, tbl: SymbolTable): () == return

    bindDeclare(ab, tbl): () ==
	return

    bindConditions(ab, tbl): () ==
	return

    bindImport(ab, tbl): () ==
        import from AbSynImport
	addImport!(tbl, importOrigin(asImport ab))
