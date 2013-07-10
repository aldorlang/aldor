--> testrun -l axllib
--> testcomp -O
--> testrun -O -l axllib

#include "axllib"

Foo(x:String == "def"): with { foo:() -> String; } == add { foo():String == x; }

T1(): () == {
	import from Foo("ghi");
	print << "foo() = " << foo() << newline;
}

T1();

T2(): () == {
	import from Foo();
	print << "foo() = " << foo() << newline;
}

T2();

