#include "comp.as"
#include "aldorio.as"
#pile

AbSynTag: Join(HashType, OutputType) with
    name: % -> String
    tag: String -> %
== add
    Rep == Record(name: String)
    import from Rep
    
    tag(a: String): % == per [a]
    name(tag: %): String == (rep tag).name
    hash(tag: %): MachineInteger == hash rep(tag).name
    (a: %) = (b: %): Boolean == name a = name b
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
    import from String
    _add: AbSynTag == tag "add"
    apply: AbSynTag == tag "apply"
    comma: AbSynTag == tag "comma"
    declare: AbSynTag == tag "declare"
    _define: AbSynTag == tag "define"
    id: AbSynTag == tag "id"
    _if: AbSynTag == tag "if"
    label: AbSynTag == tag "label"
    lambda: AbSynTag == tag "lambda"
    literal: AbSynTag == tag "literal"
    sequence: AbSynTag == tag "sequence"
    _with: AbSynTag == tag "with"

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
        stdout << "(" << name tag ab << " " << [name tag abn for abn in children ab]
        import from Id
        body := [sexpr(abn) for abn in rep(ab).children]
	attrs := if field?(rep(ab).tbl, any(id)) then cons(sexpr (-string rep(ab).tbl.id), nil) else nil
	attrs := if field?(rep(ab).tbl, any(literal)) then cons(sexpr rep(ab).tbl.literal, nil) else attrs
        sx := cons(sexpr (-name rep(ab).tag), append(attrs, body))
	stdout << ")" << newline
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
    