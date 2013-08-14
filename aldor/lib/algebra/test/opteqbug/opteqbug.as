--------------------------------- opteqbug.as -------------------------------------
--
-- WE FOUND 94 OCCURENCES OF THIS BUG INSIDE libalgebra!
--
-- THE CODE GENERATED AT -Q2 SHOWS THAT THE CALL TO =$% IS TRANSFORMED TO fiBINT_EQ
-- aldor -q2 -fc opteqbug.as
-- static FiWord
-- CF4_foo(FiEnv e1, FiWord P0_x)
-- {
--         FiBool T1;
--         FiWord T0;
--         PFmt8 l1;
--         l1 = (PFmt8) fiEnvLevel(e1);
--         T0 = fiCCall1(FiWord, l1->X1_bar, P0_x);
--         fiBINT_EQ(T1, FiBool, (FiBInt) T0, (FiBInt) P0_x);
--         return (FiWord) T1;
-- }
-- 
-- THE CODE GENERATED AT -Q1 IS OK:
-- aldor -q1 -fc opteqbug.as
-- static FiWord
-- CF4_foo(FiEnv e1, FiWord P0_x)
-- {
--         FiWord T0;
--         PFmt8 l1;
--         l1 = (PFmt8) fiEnvLevel(e1);
--         T0 = fiCCall1(FiWord, l1->X1_bar, P0_x);
--         return fiCCall2(FiWord, l1->X2__EQ_, T0, P0_x);
-- }
--
--
-- THE BUG DOES NOT OCCUR WITH #include "aldor" (SMALLER LIBRARY)
#include "algebra"

define Foo(R:PrimitiveType):Category == PrimitiveType with {
	foo: % -> Boolean;
	bar: % -> %;
	default foo(x:%):Boolean == bar(x) = x;
}

