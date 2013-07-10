#include "aldor"
#include "aldorio"
#include "debuglib"

start!()$NewDebugPackage;

main(): () == {
	import from MachineInteger;
	x: MachineInteger := 1;

	stdout << x << newline;
}

main();
