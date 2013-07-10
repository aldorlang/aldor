--> testrun -O -l axllib
--> testrun -l axllib
--> testcomp

#include "axllib"

Foo(x:String == "def"): with { foo:() -> String; } == add { foo():String == x; }

import from Foo("hey I gave a value for the argument!");

print << "foo() = " << foo() << newline;

