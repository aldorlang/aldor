#include "axllib"
#include "debuglib"

SI==>SingleInteger;

start!()$NewDebugPackage;

main(p:SI): SI == {
	import from SingleInteger;

	local x: SI := p*p;
	y: SI := foo(x);

	query();
	query(): () == {print << x <<newline;}
	hin(p1:SI): SI == {
		print << x << newline;
		print << y << newline;
		foo(p1);
	}

	hin(y);
}

foo(f:SI): SI == {
	f*a;
}

a:SI := 2;

main(a);
