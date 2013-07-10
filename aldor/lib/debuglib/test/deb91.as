#include "axllib"
#include "debuglib"

SI==>SingleInteger;


main(p:SI): SI == {
	import from SingleInteger;

	local x: SI := p*p;
	y: SI := foo(x);

	hin(p1:SI): SI == {
		print << x << newline;
		print << y << newline;
		query();
		query():() == {print << x << newline;}
		foo(p1);
	}

	hin(y);
}

foo(f:SI): SI == {
	f*a;
}

a:SI := 2;

main(a);

