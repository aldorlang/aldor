#include "foamlib"
#include "assertlib"
#pile

Foo: with
    new: String -> %
    foo: % -> String
    bar: % -> String
== add
    Rep == Cross(String, String)
    new(n: String): % == (n, n)@Rep pretend %
    foo(c: %): String ==
        (a, b) := rep c
	a

    bar(c: %): String == bar(rep c)

    bar(c: Rep): String ==
        (a, b) := c
	b

import from Assert String

test(): () ==
    import from Foo, String
    assertEquals("xx", foo new "xx")
    assertEquals("xx", bar new "xx")

test()