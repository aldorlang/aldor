#include "foamlib"
#pile

foo(): MachineInteger == never

bar(): MachineInteger ==
       stdout << foo() + foo() << newline
       22
