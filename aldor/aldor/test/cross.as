#include "foamlib"
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
	a

test(): () ==
    import from Foo, String
    print << foo(new("xx")) << newline
    print << bar(new "xx") << newline

test()