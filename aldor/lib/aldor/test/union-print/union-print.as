#include "aldor"
#include "aldorio"

U ==> Union(a: Integer, b: String);

test1(): () == {
	u: U := [12];
	stdout << u << newline;
}

test2(): () == {
	import from Assert String;
	import from String;

        u: U := [12];
	buf: StringBuffer := new();
	out: TextWriter := buf::TextWriter;

	out << u;

	assertEquals("[12@AldorInteger]", string buf);
}
test1();
test2();
