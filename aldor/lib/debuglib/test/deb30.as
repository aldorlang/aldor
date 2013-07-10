#include "axllib"
#include "debuglib"

SI==>SingleInteger;
ARR ==> Array;

start!()$NewDebugPackage;


main(p:SI): SI == {
	import from SI;
	import from ARR SI;

		query();
		query():() == { print<<x<<newline;}
	local x: SI := p*p;
	y: SI := foo(x);
	z:String:= "ORCCA";
	w:ARR SI:= new(2,2);

	hin(p1:SI): SI == {
		i := "s";
		print << w << newline;
		foo(p1);
	}

	hin(y);
}

foo(f:SI): SI == {
	f*a;
}

a:SI := 2;

main(a);
