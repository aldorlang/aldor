#include "aldor"
#include "aldorio"
#pile

BindingSet: Join(PrimitiveType, OutputType) with
    empty: () -> %
    one: String -> %
    bracket: Generator Cross(String, Integer) -> %
    keys: % -> Generator String
    path: (%, String) -> List Integer
== add
    Rep == HashTable(String, List Integer)
    import from Rep, Integer, List Integer, List String

    empty(): % == per table()
    one(name: String): % ==
        pair := (name, [1])
	per [pair]

    (a: %) = (b: %): Boolean == rep(a) = rep(b)
    [g: Generator Cross(String, Integer)]: % ==
        import from MachineInteger
        qq: List Cross(String, Integer) := [g]
	tt: Rep := table()
	for (id, n) in qq repeat tt.id := [n]
	per tt

    (o: TextWriter) << (bs: %): TextWriter == o << [keys rep(bs)]

    keys(bs: %): Generator String == keys rep bs
    path(bs: %, s: String): List Integer == rep(bs).s

#if ALDORTEST
#include "aldor"
#include "aldorio"
#include "comp"
#pile

test(): () ==
    import from BindingSet, String, List Integer, Integer
    import from Assert List Integer
    s: BindingSet := one("hello")
    stdout << path(s, "hello") << newline
    assertEquals("", [1], path(s, "hello"))

test2(): () ==
    import from BindingSet, String, List Integer, Integer
    import from Assert List Integer
    l: List Cross(String, Integer) := [("a", 1), ("b", 2)]
    stdout << [x for x in l] << newline

test()
test2()
#endif
