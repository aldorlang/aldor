-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 20-Jan-2006
-- Aldor version 1.0.3 for LINUX(glibc2.3)
-- Subject: return and -wdebug segfault

-- The compilation with
--   aldor -laldor -Wdebug -DC1 xxx.as
-- segfaults while
--   aldor -laldor         -DC1 xxx.as
--   aldor -laldor -Wdebug     xxx.as
-- gives no error message.
#include "aldor"
macro UTERM == Record(exp: MachineInteger, coef: R);
UP(R: IntegerType): with {
	exponent: % -> MachineInteger;
} == add { 
	Rep == List UTERM;
	import from UTERM, Rep;
	exponent(x: %): MachineInteger == {
                y := rep x;
                n := #y;
                u: UTERM := y.n;
#if C1
	        return u.exp;
#else
		u.exp;
#endif
        }
}
