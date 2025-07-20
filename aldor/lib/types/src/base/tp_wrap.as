#include "types"
#include "aldorio"
#pile
-- Idea is that we can Wrap a value with a bunch of properties

Item(X: with): with
    new: String -> %
    name: % -> String
== add
    Rep == String

    new(s: String): % == per s
    name(t: %): String == rep t

ItemAndDefault(X: with): with
    new: (String, X) -> %
    name: % -> String
    val: % -> X
== add
    Rep == Cross(String, X)

    new(s: String, x: X): % == per pair(s, x)
    local pair(s: String, x: X): Cross(String, X) == (s, x)

    name(t: %): String ==
        (s, x) := rep t
        s
    val(t: %): X == 
        (s, x) := rep t
        x

Wrapped(T: with): with
    wrap: T -> %
    value: % -> T
    get: (%, String) -> Pointer
    set!: (%, String, Pointer) -> ()
    isSet?: (%, String) -> Boolean
    if T has SExpressionOutputType then SExpressionOutputType
== add
    Rep == Record(val: T, tbl: HashTable(String, Pointer))
    import from Rep, HashTable(String, Pointer)
    import from Pointer
    
    wrap(t: T): % == per [t, []]
    value(w: %): T == rep(w).val
    isSet?(w: %, s: String): Boolean ==
        p: Partial Pointer := find(s, rep(w).tbl)
	not failed? p

    get(w: %, s: String): Pointer == rep(w).tbl.s
    set!(w: %, s: String, p: Pointer): () ==
        rep(w).tbl.s := p

    if T has SExpressionOutputType then
        sexpression(s: %): SExpression == sexpression(rep(s).val)

WrappedCategory: Category == with
    get: (%, String) -> Partial Pointer
    set!: (%, String, Pointer) -> ()
    isSet?: (%, String) -> Boolean

Lookup(T: with, X: with): with
    apply: (Item X, Wrapped T) -> X
    set!: (Item X, Wrapped T, X) -> ()
    isSet?: (Item X, Wrapped T) -> Boolean
    export from Item X, Wrapped T
== add
    import from Wrapped T, Item X

    isSet?(i: Item X, t: Wrapped T): Boolean == isSet?(t, name i)
    apply(i: Item X, w: Wrapped T): X == get(w, name i) pretend X
    set!(i: Item X, w: Wrapped T, x: X): () == w.(name i) := (x pretend Pointer)
    
WLookup(T: WrappedCategory, X: with): with
    apply: (Item X, t: T) -> X
    set!: (Item X, t: T, X) -> ()

    apply: (ItemAndDefault X, t: T) -> X
    set!: (ItemAndDefault X, t: T, X) -> ()
    isSet?: Item X -> T -> Boolean
== add
    import from T
    import from Item X
    import from Partial Pointer

    isSet?(i: Item X)(t: T): Boolean == isSet?(t, name i)
    set!(i: Item X, t: T, x: X): () == set!(t, name i, x pretend Pointer)
    apply(i: Item X, t: T): X ==
        pv := get(t, name i)
	failed? pv => never
	(retract pv) pretend X

    set!(i: ItemAndDefault X, t: T, x: X): () == set!(t, name i, x pretend Pointer)

    apply(i: ItemAndDefault X, t: T): X ==
        v: Partial Pointer := get(t, name i)
	failed? v => val i
	(retract v) pretend X
    
    