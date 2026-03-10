#include "foamlib"

Foo: with { f: () -> () } == add { f(): () == never }

Foo2(T: with): with { f: () -> T } == add { f(): T == never }
