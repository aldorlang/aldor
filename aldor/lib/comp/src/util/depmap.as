#include "aldor.as"
#include "aldorio.as"
#pile

Field(X: with): PrimitiveType with
    fieldEquals: (%, Y: with, Field Y) -> Boolean
    name: % -> String
    any: % -> AnyField
    field: String -> %
    apply: (DepTable, %) -> X
    set!: (DepTable, %, X) -> ()
    field?: (DepTable, %) -> Boolean
== add
   import from String
   Rep == String

   field(x: String): % == per x
   (a: %) = (b: %): Boolean == name a = name b

   fieldEquals(f: %, Y: with, f2: Field Y): Boolean ==
       name f = name(f2)$Field(Y) -- BUG: $ isn't needed

   name(f: %): String == rep f
   any(f: %): AnyField == new(X, f)
   apply(t: DepTable, f: %): X == field(t, X, f)
   set!(t: DepTable, f: %, value: X): () == add!(t, X, f, value)
   field?(t: DepTable, f: %): Boolean == field?(t, any f)

-- Can we lose this???
AnyField: HashType with
    new: (xT: with, Field xT) -> %
== add
    Rep ==> Cross(cT: with, Field cT)
    import from String

    new(xT: with, f: Field xT): % == (xT, f) pretend %

    (a: %) = (b: %): Boolean ==
        name a = name b
    
    name(a: %): String ==
        name(T: with, f: Field T): String == name f
    	name rep a
	
    hash(a: %): MachineInteger == hash name a

-- TODO: Lose!
Something: with
    thing: (X: with, Field X, X) -> %;
    value: (%, T: with, Field T) -> T
    field: % -> AnyField
== add
    Rep ==> Cross(Tx: with, Field Tx, val: Tx)
    
    value(x: %, T: with, field: Field T): T ==
        (yT, yField, yVal) == rep x
	if fieldEquals(field, yT, yField) then yVal pretend T else never
    thing(X: with, f: Field X, x: X): % == (X, f, x) pretend %
    field(a: %): AnyField == field rep a

    field(Tx: with, f: Field Tx, val: Tx): AnyField == any f

DepTable: with
    field: (%, T: with, Field T) -> T
    field?: (%, AnyField) -> Boolean
    table: () -> %
    add!: (%, T: with, Field T, T) -> ()
== add
    Rep == HashTable(AnyField, Something)
    import from Rep
    import from AnyField, Something

    table(): % == per table()
    field?(tbl: %, f: AnyField): Boolean ==
        import from Partial Something
        not failed? find(f, rep tbl)

    field(tbl: %, T: with, f: Field T): T ==
        thing := (rep(tbl)).(any f)
	value(thing, T, f)
	
    add!(tbl: %, T: with, f: Field T, value: T): () ==
        rep(tbl).(any f) := thing(T, f, value)

----------------------------------------
#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

foo(): () ==
    import from String, Something, Integer
    age: Field Integer := field "argC"
    firstName: Field String := field "firstName"
    wheels: Field Integer := field "wheels"
    tbl: DepTable := table()
    add!(tbl, String, firstName, "fred")
    add!(tbl, Integer, age, 22)
    n := field(tbl, String, firstName)
    a := field(tbl, Integer, age)
    stdout << n << newline    
    stdout << a	 << newline
    stdout << field?(tbl, age) << newline
    stdout << field?(tbl, wheels) << newline

foo()
#endif
