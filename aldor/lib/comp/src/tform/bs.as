#include "comp"
#include "aldorio"
#pile

BindingSet: Join(PrimitiveType, OutputType) with
    empty: () -> %
    empty?: % -> Boolean
    one: Id -> %
    bracket: Generator Cross(Id, List MachineInteger) -> %
    keys: % -> Generator Id
    path: (%, Id) -> List MachineInteger
    member?: (Id, %) -> Boolean
== add
    Rep == HashTable(Id, List MachineInteger)
    import from Rep, MachineInteger, List MachineInteger, List Id, Id

    empty(): % == per table()
    empty?(bs: %): Boolean ==
        empty? rep bs
    one(name: Id): % ==
        pair := (name, [1])
	per [pair]

    (a: %) = (b: %): Boolean == rep(a) = rep(b)

    [g: Generator Cross(Id, List MachineInteger)]: % ==
	tt: Rep := table()
	for (id, path) in g repeat tt.id := path
	per tt

    (o: TextWriter) << (bs: %): TextWriter == o << [keys rep(bs)]

    keys(bs: %): Generator Id == keys rep bs
    path(bs: %, s: Id): List MachineInteger ==
        rep(bs).s

    member?(id: Id, bs: %): Boolean ==
        import from Partial List MachineInteger
        not failed? find(id, rep bs)

#if ALDORTEST
#include "aldor"
#include "aldorio"
#include "comp"
#pile

string(l: Literal): Id == id string l

test(): () ==
    import from BindingSet, Id, List MachineInteger, MachineInteger
    import from Assert List MachineInteger
    s: BindingSet := one("hello")
    stdout << path(s, "hello") << newline
    assertEquals("", [1], path(s, "hello"))

test2(): () ==
    import from BindingSet, Id, List MachineInteger, MachineInteger
    import from Assert List MachineInteger

    l: List Cross(Id, List MachineInteger) := [("a", [1]), ("b", [2])]
    bs: BindingSet := [x for x in l]
    assertTrue(member?("a", bs))
    assertTrue(member?("b", bs))
    assertFalse(member?("x", bs))

test3(): () ==
    import from BindingSet, Id, List MachineInteger, MachineInteger
    import from Assert List MachineInteger
    assertTrue(empty? empty())
    assertFalse(empty? one("hello"))

test()
test2()
test3()
#endif
