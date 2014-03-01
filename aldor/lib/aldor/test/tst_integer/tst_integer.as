#include "aldor.as"
#include "aldorio.as"

import from MachineInteger, Integer;

a:MachineInteger := 1551877902;    -- smaller than max()@MachineInteger
b:Integer := a::Integer;
c:MachineInteger := machine(b);

stdout << "a,b,c = " << a << "," << b << "," << c << newline;

import from Assert Integer;
import from Assert MachineInteger;

assertEquals(a, c);
assertEquals(machine(b), c);


