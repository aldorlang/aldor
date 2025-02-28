#include "types"
#include "aldorio"
#pile

Env: with
    empty: () -> %
    meanings: (%, Symbol) -> List TypeTerm
    put!: (%, Symbol, TypeTerm) -> ()
    putParamType!: (%, Symbol, Symbol) -> ()
    push: % -> %
    param?: (%, Symbol) -> Boolean
    paramType: (%, Symbol) -> Symbol
== add
    Tbl == HashTable(Symbol, List TypeTerm)
    PTbl == HashTable(Symbol, Symbol)
    Lvl == Record(tbl: Tbl, params: PTbl)
    Rep == List Lvl
    import from Rep, Tbl, PTbl, Lvl, List TypeTerm
    import from Fold2 List TypeTerm
    import from Partial Symbol
    
    empty(): % == per [emptyLevel()]

    local emptyLevel(): Lvl == [forget(defVal), []]
    local defVal(k: Symbol): List TypeTerm == []

    meanings(e: %, sym: Symbol): List TypeTerm == (append!,[])/(lookup(lvl,sym) for lvl in rep(e))

    local lookup(lvl: Lvl, sym: Symbol): List TypeTerm ==
        stdout << "Lookup " << sym << " --> " << lvl.tbl.sym << newline
	lvl.tbl.sym
	
    put!(e: %, sym: Symbol, t: TypeTerm): () ==
        (first rep(e)).tbl.sym := cons(t, (first rep(e)).tbl.sym)

    putParamType!(e: %, sym: Symbol, tsym: Symbol): () ==
        first(rep(e)).params.sym := tsym

    push(e: %): % == per cons(emptyLevel(), rep e)

    param?(e: %, sym: Symbol): Boolean ==
        not failed? find(sym, first(rep(e)).params)

    paramType(e: %, sym: Symbol): Symbol == first(rep(e)).params.sym
