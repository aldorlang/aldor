#include "comp"
#pile

SymeKind: Join(PrimitiveType, OutputType) with
    description: % -> String
    isAssignable?: % -> Boolean
    kind: (desc: String, isAssignable: Boolean == false) -> %
== add
    Rep ==> Record(desc: String, isAssignable: Boolean)
    import from Rep

    kind(desc: String, isAssignable: Boolean): % == per [desc, isAssignable]
    description(kind: %): String == rep(kind).desc
    isAssignable?(kind: %): Boolean == rep(kind).isAssignable

    (a: %) = (b: %): Boolean == rep(a).desc = rep(b).desc
    (o: TextWriter) << (k: %): TextWriter == o << rep(k).desc

SymeKinds: with
    EXPORT: SymeKind
    IMPORT: SymeKind
    LEX: SymeKind
    PARAM: SymeKind
    VAR: SymeKind

    export from SymeKind
== add
    import from String
    EXPORT: SymeKind == kind("export")
    IMPORT: SymeKind == kind("import")
    LEX: SymeKind == kind("lex", true)
    PARAM: SymeKind == kind("param", true)
    VAR: SymeKind == kind("var", true)

SymeField(X: with): with
    apply: (Syme, Field X) -> X
    set!: (Syme, Field X, X) -> ()
    export from Field X
== add
    apply(syme: Syme, f: Field X): X == fields(syme).f
    set!(syme: Syme, f: Field X, x: X): () == fields(syme).f := x

ALL__FIELDS ==>
    FIELD(id, Id, "id")
    FIELD(type, TForm, "type")
    FIELD(kind, SymeKind, "kind")
    FIELD(level, SymbolTableLevel, "level")

SymeFields: with
    FIELD(name, type, nameText) ==>
        name: Field(type)
        export from SymeField(type)

    ALL__FIELDS
== add
    import from String
    FIELD(name, type, nameText) ==>
        name: Field(type) == field nameText
    ALL__FIELDS

+++ Syme represents a symbol with associated type and
+++ context.
Syme: Join(HashType, OutputType) with
    newExport: (Id, TForm, SymbolTableLevel) -> %
    newImport: (Id, TForm, SymbolTableLevel) -> %
    newLex: (Id, TForm, SymbolTableLevel) -> %
    newParam: (Id, TForm, SymbolTableLevel) -> %
    newVar: (Id, TForm, SymbolTableLevel) -> %

    fields: % -> DepTable
== add
    Rep == DepTable
    import from Rep
    import from String, Id
    import from SymeKinds, SymeFields, SymeKinds

    fields(syme: %): DepTable == rep syme
    hash(syme: %): MachineInteger == hash syme.id

    newSyme(theKind: SymeKind, theId: Id, theTf: TForm, theLevel: SymbolTableLevel): % ==
        tbl := table()
	tbl.kind := theKind
	tbl.id := theId
	tbl.level := theLevel
	per tbl
	
    newExport(id: Id, tf: TForm, lvl: SymbolTableLevel): % == newSyme(EXPORT, id, tf, lvl)
    newImport(id: Id, tf: TForm, lvl: SymbolTableLevel): % == newSyme(IMPORT, id, tf, lvl)
    newLex(id: Id, tf: TForm, lvl: SymbolTableLevel): % == newSyme(LEX, id, tf, lvl)
    newParam(id: Id, tf: TForm, lvl: SymbolTableLevel): % == newSyme(PARAM, id, tf, lvl)
    newVar(id: Id, tf: TForm, lvl: SymbolTableLevel): % == newSyme(VAR, id, tf, lvl)

    (o: TextWriter) << (syme: %): TextWriter ==
        o << "{ (" << syme.kind << ") " << syme.id << "}"
    
    (syme1: %) = (syme2: %): Boolean ==
        import from TForm, SymbolTableLevel
        syme1.id = syme2.id
	    and syme1.type = syme2.type
	    and syme1.level = syme2.level
