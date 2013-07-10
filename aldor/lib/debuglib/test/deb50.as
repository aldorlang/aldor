#include "axllib"
#include "debuglib"

SI==>SingleInteger;
start!()$NewDebugPackage;
import from SI;
main(p:SI): SI == {
	--import from SI;

	x: SI := p*p;
	x;
}

main(1);
