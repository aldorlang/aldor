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

-- FIXME: Can't be local as it's used in exported types
AbField(X: with): with
    apply: (AbSyn, Field X) -> X
    set!: (AbSyn, Field X, X) -> ()
    export from Field X
== add
    import from Field X
    apply(ab: AbSyn, f: Field X): X == tbl(ab).f
    set!(ab: AbSyn, f: Field X, value: X): () == tbl(ab).f := value

AbSynFields: with
    id: Field Id
    literal: Field String
    symbolTable: Field SymbolTable

    symbolTable: (ab: AbSyn, parent: SymbolTable) -> SymbolTable;

    export from AbField Id, AbField String, AbField SymbolTable
== add
    import from String
    id: Field Id == field "id"
    literal: Field String == field "literal"
    symbolTable: Field SymbolTable == field "symbolTable"

    symbolTable(ab: AbSyn, parent: SymbolTable): SymbolTable ==
        if field?(tbl ab, symbolTable) then ab.symbolTable else parent

AbSyn: OutputType with
    _add: () -> % -- FIXME!
    apply: Tuple % -> %
    comma: Tuple % -> %
    declare: (%, %) -> %
    _define: (%, %) -> %
    id: Id -> %
    label: (Id, %) -> %
    lambda: (AbSyn, AbSyn, AbSyn) -> %
    literal: String -> %
    sequence: Tuple % -> %
    _with: () -> % -- FIXME!

    tbl: % -> DepTable
    tag: % -> AbSynTag
    children: % -> List %
    
    export from AbSynTags, AbSynTag, AbSynFields
== add
    Rep ==> Record(tag: AbSynTag, tbl: DepTable, children: List %)
    import from Rep
    import from AbSynTags, AbSynTag, DepTable, List %, Something, AbSynFields
    import from Field Id

    tag(ab: %): AbSynTag == rep(ab).tag
    tbl(ab: %): DepTable == rep(ab).tbl
    children(ab: %): List % == rep(ab).children

    local tbl(anId: Id): DepTable ==
        t := table()
	t.id := anId
	t

    local tbl(text: String): DepTable ==
        t := table()
	t.literal := text
	t

    id(anId: Id): % == per [id, tbl anId, []]
    literal(text: String): % == per [id, tbl text, []]
    declare(val: %, type: %): % == per [declare, table(), [val, type]]
    sequence(t: Tuple %): % == per [sequence, table(), [t]]
    _define(lhs: %, rhs: %): % == per [_define, table(), [lhs, rhs]]
    comma(t: Tuple %): % == per [comma, table(), [t]]
    apply(t: Tuple %): % == per [apply, table(), [t]]
    label(id: Id, ab: %): % == per [label, tbl(id), [ab]]
    lambda(type: AbSyn, ret: AbSyn, body: AbSyn): % == per [lambda, table(), [type, ret, body]]
    _with(): % == per [_with, table(), []]
    _add(): % == per [_add, table(), []]

    sexpr(ab: AbSyn): SExpression ==
        import from List String, Symbol
        import from Id
        body := [sexpr(abn) for abn in rep(ab).children]
	attrs := if field?(rep(ab).tbl, any(id)) then cons(sexpr (-string rep(ab).tbl.id), nil) else nil
	attrs := if field?(rep(ab).tbl, any(literal)) then cons(sexpr rep(ab).tbl.literal, nil) else attrs
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

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

import from Id
string(s: Literal): AbSyn ==
    import from Id
    id id string s

string(s: Literal): Id == id string s

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
    root := bind(absyn)$SymbolTableBinder
    stdout << root << newline

    -- Foo(X: with, l: List X): MachineInteger == #l
    absyn := sequence(_define(declare("Foo", apply("Map", comma(declare("X", _with()),
    	     				     		        declare("l", apply("List", "X"))),
							  "MachineInteger")),
			      lambda(comma(declare("X", _with()),
    	     				     		        declare("l", apply("List", "X"))),
							  "MachineInteger",
				     label("Foo", apply("size", "l")))))
    stdout << absyn << newline
    root := bind(absyn)$SymbolTableBinder
    stdout << root << newline

testAbSyn()

#endif
    