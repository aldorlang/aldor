#include "comp.as"
#pile

SymbolTableType: Join(PrimitiveType, OutputType) with
    type: String -> %
    name: % -> String
== add
    Rep == Record(name: String)
    import from Rep
    type(s: String): % == per [s]
    name(x: %): String == rep(x).name
    
    (a: %) = (b: %): Boolean == name a = name b
    (o: TextWriter) << (a: %): TextWriter == o << name a;
    
SymbolTableTypes: with
    topLevel: SymbolTableType
    file: SymbolTableType
    comma: SymbolTableType
    lambda: SymbolTableType
== add
    import from String
    topLevel: SymbolTableType == type "topLevel"
    file: SymbolTableType == type "file"
    comma: SymbolTableType == type "comma"
    lambda: SymbolTableType == type "lambda"

StabField(X: with): with
    apply: (SymbolTable, Field X) -> X
    set!: (SymbolTable, Field X, X) -> ()
== add
    import from Field X
    apply(tbl: SymbolTable, f: Field X): X == fields(tbl).f
    set!(tbl: SymbolTable, f: Field X, x: X): () == fields(tbl).f := x

SymbolTable: OutputType with
    type: % -> SymbolTableType
    parent: % -> %
    children: % -> List %
    root?: % -> Boolean

    root: () -> %
    new: (%, SymbolTableType) -> %
    fields: % -> DepTable
    export from SymbolTableTypes
== add
    Rep == Record(t: SymbolTableType, fields: DepTable, parent: Partial %, children: List %)
    import from Rep
    import from DepTable, SymbolTableTypes
    default tbl, prnt: %

    root(): % == per [topLevel, table(), failed, []]
    fields(tbl): DepTable == rep(tbl).fields
    
    new(prnt, type: SymbolTableType): % ==
        lvl := per [type, table(), [prnt], []]
	rep(prnt).children := cons(lvl, rep(prnt).children)
	lvl

    type(tbl): SymbolTableType == rep(tbl).t
    parent(tbl): % == retract rep(tbl).parent
    root?(tbl): Boolean == failed? rep(tbl).parent
    children(tbl): List % == rep(tbl).children

    sexpr(tbl): SExpression ==
        import from Symbol
        cons(sexpr (-name type tbl),
                 [sexpr(child) for child in rep(tbl).children])

    (o: TextWriter) << (tbl: %): TextWriter ==
    	import from SExpression
        o << sexpr tbl

