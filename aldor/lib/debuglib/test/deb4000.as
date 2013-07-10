#include "aldor"
#include "debuglib"

SI==>MachineInteger;
STR==> String;

import from SI;
--start!()$NewDebugPackage;

main(p:SI): (SI, SI) == {
	x:SI := 1;	
	(p, x);
}

main(1);




