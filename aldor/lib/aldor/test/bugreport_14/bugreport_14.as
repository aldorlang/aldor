-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 18-Oct-2005
-- Aldor version 1.0.3 for LINUX(glibc2.3)
-- Subject: Wrong function call

-- If started via
-- aldor -grun -laldor xxx.as
-- the program correctly outputs

--: BBB
--: AAA

-- Simply renaming the function SET! to set! and running via
-- aldor -grun -laldor -DC1 xxx.as
-- gives the wrong output

--: BBB
--: BBB

-- Obviously the compiler rather thinks that
-- set!(aaa, j, i, p)$AAA(P);
-- stands for the set!: (%, E, E, P) -> P function from BBB 
-- and not from AAA, although clearly aaa is of type AAA(P)
-- and not of type %.

#include "aldor"
#include "aldorio"
macro {	
	E == MachineInteger;
#if C1
	SET! == set!;
#endif
}
import from MachineInteger;

AAA(P: Type): with {
	new: () -> %;
	set!: (%, E, E, P) -> P;
} == add {
        Rep == Array PrimitiveArray Partial P;
        import from Rep;
	new(): % == per new 0;
	set!(t: %, j: E, i: E, p: P): P == {
		stdout << "AAA" << newline;
		p;
	}
}

BBB(P: Type): with {
	new: () -> %;
	SET!: (%, E, E, P) -> P;
	set!: (%, E, E, E, E, P) -> P;
} == add {
        Rep == Array Array AAA P;
        import from Rep, MachineInteger;
	new(): % == per new 0;
	SET!(t: %, y: E, x: E, p: P): P == {
		stdout << "BBB" << newline;
		p;
	}
	set!(t: %, y: E, x: E, j: E, i: E, p: P): P == {
		aaa: AAA P := new();
		set!(aaa, j, i, p)$AAA(P);
	}
}

main(): () == {
	ct: BBB MachineInteger := new();
	SET!(ct, 2, 1, 20104);        -- print BBB
        set!(ct, 2, 1, 1, 3, 201013); -- print AAA
}
main();
