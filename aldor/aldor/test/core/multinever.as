#include "foamlib"
#pile
Foo: with
    foo: Integer -> (String, String)
== add
    foo(n: Integer): (String, String) == never

import from Integer, Foo
(a, b) := foo 22
