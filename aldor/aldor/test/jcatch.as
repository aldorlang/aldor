#include "foamlib"
#pile

define SomeExceptionType:Category == with;
SomeException: SomeExceptionType == add;

local fn(x: MachineInteger): MachineInteger == throw SomeException

local check(f: Boolean): () == if not f then never;

local test(): () ==
    import from MachineInteger
    x := try fn(0) catch E in -1
    stdout << x << newline
    check(x = -1)
test()
