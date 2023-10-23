#include "foamlib"
#pile

zz: MachineInteger := 0
foo(): () ==
    1 = 1 =>
        stdout << "Set zz" << zzz() << newline
        zz:= 1

zzz(): MachineInteger == zz

foo()
stdout << "zz" << zz << newline

if zz = 0 then never
