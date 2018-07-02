#include "foamlib"
#pile

export ExnThrow to Foreign Java "aldor.stuff"

define SomeExceptionType:Category == with;
SomeException: SomeExceptionType == add;

ExnThrow: with
    fn: MachineInteger -> MachineInteger
    fn2: MachineInteger -> MachineInteger
    fn3: MachineInteger -> MachineInteger
== add
    fn(x: MachineInteger): MachineInteger == throw SomeException
    fn2(x: MachineInteger): MachineInteger == if x = 0 then throw SomeException else x
    fn3(x: MachineInteger): MachineInteger ==
        while x >= 0 repeat
	       x = 0 => throw SomeException
	       x := x - 1
	return x
