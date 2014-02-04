-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 17-Oct-2005
-- Aldor version 1.0.3 for LINUX(glibc2.3)
-- Subject: Conditional exports

-- Compile with
-- aldor -fx -laldor xxx.as
-- and run via calling 'xxx'.

-- The program crashes with
--: Looking in List(MyInt) for = with code 410721090
--: Unhandled Exception: RuntimeError()
--: Export not found

-- Of course this program must crash since MyInt does not implement
-- PrimitiveType and therefore no equality test.

-- Unfortunately, this program is NOT rejected by the compiler,
-- although it actually should reject it.

-- List inherits the equality test from the default implementation
-- of = in BoundedFiniteLinearStructureType. There, however, it says:
-- if T has PrimitiveType then {
--   (a:%) = (b:%):Boolean == {
--     import from Z, T;
--     #a ~= #b => false;
--     for x in a for y in b repeat x ~= y => return false;
--     true;
--   }
-- }
-- The code from the category has been overridden by the direct
-- implementation in List via
-- if T has PrimitiveType then {
--   (l1:%) = (l2:%):Boolean == {
--     while ~empty?(l1) repeat {
--       empty? l2 or (first l1 ~= first l2) => return false;
--       l1 := rest l1;
--       l2 := rest l2;
--     }
--     empty? l2;
--   }
-- }
-- so the compiler should not know of = for List MyInt, because
-- in both cases the implementation requires the parameter type T
-- to have PrimitiveType which MyInt clearly does not satisfy.

#include "aldor"
macro Z == AldorInteger;
MyInt: with {
        coerce: Z -> %;	
} == add {
        Rep == Z;
        import from Z;
        coerce(z: Z): % == per z;
}
main():() == {
	import from Z, MyInt, List MyInt, TextWriter, Character;
	try { l: List MyInt := [1 :: MyInt];
	      stdout << (l = l) << newline;
        }
	catch E in {
	   import from String;
	   stdout << "This test should fail to compile.  An existing bug means that an exception is thrown" << newline
	}
}
main();
