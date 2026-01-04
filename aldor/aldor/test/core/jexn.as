#include "foamlib"
#pile

APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

JavaExceptionType: Category == with;

import
    ExceptionExample: with
        new: MachineInteger -> % throw JavaExceptionType
	APPLY(decrement, () -> () throw JavaExceptionType)
	random: () -> % throw JavaExceptionType
#if 0
        new: MachineInteger -> %
	APPLY(decrement, () -> ())
	random: () -> %
#endif
	APPLY(value, () -> MachineInteger)
    from Foreign Java "aldor.test"

import from ExceptionExample
import from MachineInteger

testNew(): () ==
    ee: ExceptionExample := new(2) pretend ExceptionExample
    stdout << ee.value() << newline

testStatic(): () ==
    r: ExceptionExample := random()$ExceptionExample pretend ExceptionExample
    stdout << r.value() << newline

testMethod(): () ==
    ee := new(4)$ExceptionExample pretend ExceptionExample
    ee.decrement()
    stdout << ee.value() << newline

testNew()
testStatic()
testMethod()
