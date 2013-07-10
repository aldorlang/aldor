#include "aldor"
#include "debuglib"

SI==>MachineInteger;
STR==> String;

import from SI;
start!()$NewDebugPackage;

main(p:SI): SI == {
	import from String, Character;
	import from Array SI, TextWriter;

	local x: SI := p*p;
	y: SI := foo(x);

	z: STR := "ORCCA @ UWO";
	w: Array SI := new(2, x);	
	
	hin(p1:SI): SI == {
		stdout << z << newline;
		stdout << w << newline;
		foo(p1);
	}

	if (x = 1) then
		return foo(x);

	hin(y);
}
foo(f:SI): SI == {
	f*a;
}

a:SI := 2;

main(a);




