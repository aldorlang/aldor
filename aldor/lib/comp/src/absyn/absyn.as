#include "comp.as"
#include "aldorio.as"
#pile

AbSynTag: Join(HashType, OutputType) with
    name: % -> String
    tag: (n: MachineInteger, String) -> %
== add
    Rep == Record(n: MachineInteger, name: String)
    import from Rep
    
    tag(n: MachineInteger, a: String): % == per [n, a]
    name(tag: %): String == (rep tag).name
    hash(tag: %): MachineInteger == hash rep(tag).n
    (a: %) = (b: %): Boolean == rep(a).n = rep(b).n

    (o: TextWriter) << (tag: %): TextWriter == o << name tag;
    
AbSynTags: with
    _add: AbSynTag
    apply: AbSynTag
    comma: AbSynTag
    declare: AbSynTag
    _define: AbSynTag
    id: AbSynTag
    _if: AbSynTag
    label: AbSynTag
    lambda: AbSynTag
    literal: AbSynTag
    sequence: AbSynTag
    _with: AbSynTag
== add
    import from String, MachineInteger
    _add: AbSynTag == tag(1, "add")
    apply: AbSynTag == tag(2, "apply")
    comma: AbSynTag == tag(3, "comma")
    declare: AbSynTag == tag(4, "declare")
    _define: AbSynTag == tag(5, "define")
    id: AbSynTag == tag(6, "id")
    _if: AbSynTag == tag(7, "if")
    label: AbSynTag == tag(8, "label")
    lambda: AbSynTag == tag(9, "lambda")
    literal: AbSynTag == tag(10, "literal")
    sequence: AbSynTag == tag(11, "sequence")
    _with: AbSynTag == tag(12, "with")

AbSyn: OutputType with
    _add: () -> % -- FIXME!
    apply: Tuple % -> %
    apply: Generator % -> %
    comma: Tuple % -> %
    comma: Generator % -> %
    declare: (%, %) -> %
    _define: (%, %) -> %
    var: Id -> %
    label: (Id, %) -> %
    lambda: (AbSyn, AbSyn, AbSyn) -> %
    literal: String -> %
    sequence: Tuple % -> %
    _with: () -> % -- FIXME!

    fields: % -> DepTable
    tag: % -> AbSynTag
    children: % -> List %

    id: % -> Id
    literal: % -> String
    symbolTable: % -> SymbolTable
    symbolTable: (%, SymbolTable) -> SymbolTable
    set!: (%, 'symbolTable', SymbolTable) -> ();

    export from AbSynTags, AbSynTag, 'symbolTable'
    export from AbSynDeclare
== add
    Rep ==> Record(tag: AbSynTag, fields: DepTable, children: List %)
    import from Rep
    import from AbSynTags, AbSynTag, DepTable, List %
    import from Field Id

    tag(ab: %): AbSynTag == rep(ab).tag
    fields(ab: %): DepTable == rep(ab).fields
    children(ab: %): List % == rep(ab).children

    local tbl(anId: Id): DepTable ==
        t := table()
	t.id := anId
	t

    local tbl(text: String): DepTable ==
        t := table()
	t.literal := text
	t

    local id: Field Id == field "id"
    local literal: Field String == field "literal"
    local symbolTable: Field SymbolTable == field "symbolTable"
    set!(absyn: %, a: 'symbolTable', tbl: SymbolTable): () ==
        rep(absyn).fields.symbolTable := tbl

    id(absyn: %): Id == (fields absyn).id
    literal(absyn: %): String == (fields absyn).literal
    symbolTable(absyn: %): SymbolTable == (fields absyn).symbolTable
    symbolTable(absyn: %, parent: SymbolTable): SymbolTable ==
        if field?((fields absyn), symbolTable) then symbolTable absyn else parent

    var(anId: Id): % == per [id, tbl anId, []]
    literal(text: String): % == per [id, tbl text, []]
    declare(val: %, type: %): % == per [declare, table(), [val, type]]
    sequence(t: Tuple %): % == per [sequence, table(), [t]]
    _define(lhs: %, rhs: %): % == per [_define, table(), [lhs, rhs]]
    comma(t: Tuple %): % == per [comma, table(), [t]]
    apply(t: Tuple %): % == per [apply, table(), [t]]
    comma(t: Generator %): % == per [comma, table(), [t]]
    apply(t: Generator %): % == per [apply, table(), [t]]
    label(id: Id, ab: %): % == per [label, tbl(id), [ab]]
    lambda(type: AbSyn, ret: AbSyn, body: AbSyn): % == per [lambda, table(), [type, ret, body]]
    _with(): % == per [_with, table(), []]
    _add(): % == per [_add, table(), []]

    sexpr(ab: AbSyn): SExpression ==
        import from List String, Symbol
        import from Id
        body := [sexpr(abn) for abn in rep(ab).children]
	attrs := if field?(rep(ab).fields, any(id)) then cons(sexpr (-string rep(ab).fields.id), nil) else nil
	attrs := if field?(rep(ab).fields, any(literal)) then cons(sexpr rep(ab).fields.literal, nil) else attrs
        sx := cons(sexpr (-name rep(ab).tag), append(attrs, body))
	sx

    (o: TextWriter) << (ab: %): TextWriter ==
    	import from SExpression
        o << sexpr ab


AbSynApply: with
   applyOp: AbSyn -> AbSyn
   applyArgs: AbSyn -> List AbSyn
== add
    import from List AbSyn
    default ab: AbSyn

    applyOp ab: AbSyn == first children ab
    applyArgs ab: List AbSyn == rest children ab

AbSynDeclare: with
   declareId: % -> Id
   declareType: % -> AbSyn
   asDeclare: AbSyn -> %
   toAbSyn: % -> AbSyn
== add
    import from List AbSyn
    Rep == AbSyn
    import from Rep

    default decl: %
    asDeclare(ab: AbSyn): % ==
        tag ab ~= declare => never
        per ab
    toAbSyn decl: AbSyn == rep decl

    declareId decl: Id == id(rep decl)
    declareType decl: AbSyn == first rest children rep decl

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

string(s: Literal): Id == id(string(s))$Id
string(s: Literal): AbSyn == var(string(s)@Id)

import from AbSyn, SymbolTable
test0(): () ==
    ab := apply("map", comma("Literal"), comma("Integer"))
    stdout << ab << newline

test0()

testAbSyn(): () ==

    absyn := sequence(declare("integer", apply("map", comma("Literal"), comma("Integer"))),
    	             declare("f", apply("map", comma("Integer"), comma("String"))),
    	             _define(declare("a", "Integer"), literal "22"),
    	    	     apply("f", "a"));
    stdout << absyn << newline
    local file, rootTbl: SymbolTable
    rootTbl := root()$SymbolTable
    file := bind(rootTbl, absyn)$SymbolTableBinder
    stdout << file << newline

    -- Foo(X: with, l: List X): MachineInteger == #l
    absyn := sequence(_define(declare("Foo", apply("Map", comma(declare("X", _with()),
    	     				     		        declare("l", apply("List", "X"))),
							  "MachineInteger")),
			      lambda(comma(declare("X", _with()),
    	     				     		        declare("l", apply("List", "X"))),
							  "MachineInteger",
				     label("Foo", apply("size", "l")))))
    stdout << absyn << newline
    rootTbl := root()$SymbolTable
    file := bind(rootTbl, absyn)$SymbolTableBinder
    stdout << file << newline

testAbSyn()

#endif
