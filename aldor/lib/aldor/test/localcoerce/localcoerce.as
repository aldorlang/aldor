-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 23-Apr-2004
-- Aldor version 1.0.1/1.0.2 for LINUX(gcc-2_96)
-- Subject: local coerce

-- Compilation with
--   aldor xxx.as
-- works fine.
-- Compilation with
--   aldor -DBUG xxx.as
-- yields 
--: Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).

#include "aldor"

CalixData: with {
	<<: (%, MachineInteger) -> %;
	<<: MachineInteger -> % -> %;
} == add {
	Rep == Record(tw: TextWriter);
	import from Rep;
#if BUG
	local coerce(x: %): TextWriter == rep(x).tw;
#else
	coerce(x: %): TextWriter == rep(x).tw;
#endif
	(x: %) << (i: MachineInteger): % == {(x :: TextWriter) << i; x}
	(<<)(i: MachineInteger)(x: %): % == x << i;
}
