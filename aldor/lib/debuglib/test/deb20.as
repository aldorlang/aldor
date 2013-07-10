#include "axllib"
#include "debuglib"

SI==>SingleInteger;

start!()$NewDebugPackage;

main(p:SI): SI == {
	import from SingleInteger;

	x: SI := p*p;
	y: SI := foo(x);

	print << x << newline;
	print << y << newline;
	y;
}

foo(f:SI): SI == {
	f*a;
}

a:SI := 2;

main(a);
