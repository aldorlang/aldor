#include "foamlib"
#library QQ "extend1.ao"
import from QQ;
extend Foo: with { h: () -> () } == add { h(): () == never }

extend Foo2(T: with): with { h: () -> T } == add { h(): T == never }