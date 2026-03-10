#include "foamlib"
#library QQ "base.ao"
import from QQ;
extend Foo: with { g: () -> () } == add { g(): () == never }

extend Foo2(T2: with): with { g: () -> T2 } == add { g(): T2 == never }
