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
    _for: SymbolTableType
    _if: SymbolTableType
    lambda: SymbolTableType
    scope: SymbolTableType
== add
    import from String
    topLevel: SymbolTableType == type "topLevel"
    file: SymbolTableType == type "file"

    comma: SymbolTableType == type "comma"
    _for: SymbolTableType == type "for"
    _if: SymbolTableType == type "if"
    lambda: SymbolTableType == type "lambda"
    scope: SymbolTableType == type "scope"

StabField(X: with): with
    apply: (SymbolTable, Field X) -> X
    set!: (SymbolTable, Field X, X) -> ()
== add
    import from Field X, SymbolTableLevel
    apply(tbl: SymbolTable, f: Field X): X == fields(level tbl).f
    set!(tbl: SymbolTable, f: Field X, x: X): () == fields(level tbl).f := x

SymbolTable: OutputType with
    level: % -> SymbolTableLevel
    fields: % -> DepTable
    parent: % -> %
    type: % -> SymbolTableType
    root?: % -> Boolean
    root: () -> %
    new: (%, SymbolTableType) -> %

    export from SymbolTableTypes
== add
    Rep == Record(parent: Partial %, lvl: SymbolTableLevel)
    import from Rep
    import from SymbolTableTypes
    default tbl, prnt: %

    root? tbl: Boolean == failed? rep(tbl).parent
    root(): % == per [failed, newLevel(topLevel)]
    fields tbl: DepTable == fields level tbl
    type tbl: SymbolTableType == type level tbl
    parent tbl: % == retract rep(tbl).parent
    level tbl: SymbolTableLevel == rep(tbl).lvl
    fields tbl: DepTable == fields rep(tbl).lvl

    new(tbl, type: SymbolTableType): % ==
        lvl := newLevel(type)
	addChild!(level tbl, lvl)
	per [[tbl], lvl]

    (o: TextWriter) << tbl: TextWriter == o << level tbl

SymbolTableLevel: Join(PrimitiveType, OutputType) with
    type: % -> SymbolTableType
    children: % -> List %

    newLevel: SymbolTableType -> %
    fields: % -> DepTable
    addChild!: (%, %) -> ()
    export from SymbolTableType
== add
    Rep == Record(t: SymbolTableType, fields: DepTable, children: List %)
    import from Rep
    import from DepTable, SymbolTableTypes
    default tbl, prnt: %

    fields(tbl): DepTable == rep(tbl).fields
    
    newLevel(type: SymbolTableType): % ==
        per [type, table(), []]

    type(tbl): SymbolTableType == rep(tbl).t
    children(tbl): List % == rep(tbl).children

    addChild!(lvl: %, child: %): () == rep(lvl).children := cons(child, rep(lvl).children)

    sexpr(tbl): SExpression ==
        import from Symbol
        cons(sexpr (-name type tbl),
                 [sexpr(child) for child in rep(tbl).children])

    (o: TextWriter) << (tbl: %): TextWriter ==
    	import from SExpression
        o << sexpr tbl

    (tbl1: %) = (tbl2: %): Boolean ==
        import from TypedPointer %
	pointer tbl1 = pointer tbl2

