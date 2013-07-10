#include "aldor"
#include "debuglib"

import from MachineInteger;

start!()$NewDebugPackage;

main(p:MachineInteger):() == {
	--import from TextWriter, WriterManipulator;
	--stdout << p << endnl;
	import from Character;
	--stdout << p;
	newline;
}

main(1);




