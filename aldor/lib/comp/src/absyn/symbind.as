#include "comp"
#include "aldorio"
#pile

SymbolTableBinder: with
    bind: (SymbolTable, AbSyn) -> SymbolTable
== add
    import from List AbSyn
    import from SymbolTable

    bind(root: SymbolTable, whole: AbSyn): SymbolTable ==
        tbl := new(root, file)
	bind(whole, tbl)
	tbl
	
    bind(ab: AbSyn, tbl: SymbolTable): () ==
        stdout << "(Bind " << name tag ab << newline
        tag ab = lambda => bindLambda(ab, tbl)
	for abn in children ab repeat bind(abn, tbl)
        stdout << " Bind)" << newline

    bindLambda(ab: AbSyn, tbl: SymbolTable): () ==
        ab.symbolTable := new(tbl, lambda)
	for abn in children ab repeat bind(abn, symbolTable ab)

    bindDeclarationsAndDefinitions(ab: AbSyn, tbl: SymbolTable): () ==
        tag ab = _define => bindDefine(ab, tbl)
        tag ab = declare => bindDeclare(ab, tbl)
	for abn in children ab repeat bindDeclarationsAndDefinitions(abn, symbolTable(ab, tbl)) 

    bindDefine(def: AbSyn, tbl: SymbolTable): () == {}
    bindDeclare(def: AbSyn, tbl: SymbolTable): () == {}

