#include "types"
#include "aldorio"
#pile

XEnv: with
    empty: () -> %
    meanings: (%, Symbol) -> List TypeTerm
    symbolMeanings: (%, Symbol) -> List SymbolMeaning
    put!: (%, Symbol, TypeTerm) -> ()
    push: % -> %
== add
    Tbl == HashTable(Symbol, List SymbolMeaning)
    PTbl == HashTable(Symbol, Symbol)
    Lvl == Record(tbl: Tbl, params: PTbl)
    Rep == List Lvl
    import from Rep, Tbl, PTbl, Lvl, List SymbolMeaning
    import from Fold2 List SymbolMeaning
    import from Partial Symbol
    import from SymbolMeaningFields
    import from Expression
    
    empty(): % == per [emptyLevel()]

    local emptyLevel(): Lvl == [forget(defVal), []]
    local defVal(k: Symbol): List SymbolMeaning == []

    meanings(e: %, sym: Symbol): List TypeTerm == [type x for x in symbolMeanings(e, sym)]
    symbolMeanings(e: %, sym: Symbol): List SymbolMeaning == (append!,[])/(lookup(lvl,sym) for lvl in rep(e))

    local lookup(lvl: Lvl, sym: Symbol): List SymbolMeaning ==
        stdout << "Lookup " << sym << " --> " << lvl.tbl.sym << newline
	lvl.tbl.sym
	
    put!(e: %, sym: Symbol, t: TypeTerm): () ==
        syme: SymbolMeaning := new sym
	type syme := preprocess(t)
        (first rep(e)).tbl.sym := cons(syme, (first rep(e)).tbl.sym)

    push(e: %): % == per cons(emptyLevel(), rep e)

    local preprocess(t: TypeTerm): TypeTerm ==
        import from ListSet Symbol
        constant? t => t
	tbl: HashTable(Symbol, Expression) := [(k, expr(newVar()$XTypes)) for k in vars t]
	varRename(e: Expression): Expression ==
	    not term? e => e
	    v: Partial Expression := find(term e, tbl)
	    not v => e
	    retract v
	f: Expression -> Expression := mapTerms(varRename)
	forall([term v for v in entries tbl], f expr t)
