-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 28-Apr-2004
-- Aldor version 1.0.2 for LINUX(gcc-2_96)
-- Subject: missing add

-- This piece of code is certainly not syntactically correct Aldor,
-- but the compiler should not crash at it.
-- The command
--   aldor -v xxx.as
-- yields
--: Aldor version 1.0.2 for LINUX(gcc-2_96)
--: Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
#include "aldor"

macro {
	Z == MachineInteger;
	PA == PrimitiveArray;
}

AArray(n: Z, T: Type): ArrayType(T, PA T) == {
	Rep == PA T;
	import from Rep;
}
