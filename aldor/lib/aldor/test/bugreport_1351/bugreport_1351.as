-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 27-Jul-2003
-- Aldor version 1.0.0 for LINUX(gcc-2_96)
--   This is actually aldor-1.0.1-linux-i386-gmp4.bin (7684449 bytes)
--   (MD5: 29d37c14bbeb0029166ccdd6ed0cdba2)
-- Subject: [1]Wrong optimisation with -Q2

-- The functions fOkay and fWrong are basically identical. However
-- when compiled with 

-- aldor -V -I $ALDORROOT/include -Y$ALDORROOT/lib -Q2 -laldor -fc xxx.as

-- the resulting C code for fWrong shows that the optimiser has
-- removed the code for the variable R. Unfortunately the side effects
-- that result by assigning R are essential.

-- The the generated C code looks like this:
--: static FiWord
--: CF5_fOkay(FiEnv e1, FiWord P0_R, FiWord P1_C, FiClos 
--:         	P2_mustRemove_QMARK_)
--: {
--:         FiBool T5, T6;
--:         PFmt7 T4;
--:         FiWord T0_S, T1, T2, T3;
--:         fiPTR_IS_NIL(T5, FiBool, (FiPtr) P0_R);
--:         if (T5) goto L0;
--:         T0_S = P0_R;
--:         T4 = fi0RecNew(struct Fmt7 , CENSUS_Rec);
--:         T4->X0_elt = (FiWord) fiNil;
--:         T4->X1_next = P0_R;
--:         P0_R = (FiWord) T4;
--: L2:	fiPTR_IS_NIL(T6, FiBool, (FiPtr) T0_S);
--:         if (T6) goto L3;
--:         if ((FiBool) fiCCall2(FiWord, P2_mustRemove_QMARK_, ((PFmt7) T0_S)->
--:         	X0_elt, P1_C)) goto L4;
--:         P0_R = T0_S;
--:         T0_S = ((PFmt7) T0_S)->X1_next;
--:         goto L2;
--: L4:	T2 = ((PFmt7) T0_S)->X1_next;
--:         fiFree((FiPtr) T0_S);
--:         T0_S = T2;
--:         ((PFmt7) P0_R)->X1_next = T2;
--:         goto L2;
--: L3:	((PFmt7) P0_R)->X1_next = P1_C;
--:         T3 = T4->X1_next;
--:         fiFree((FiPtr) T4);
--:         T1 = T3;
--: L1:	return T1;
--: L0:	T1 = P1_C;
--:         goto L1;
--: }
--: 
--: static FiWord
--: CF6_fWrong(FiEnv e1, FiWord P0_S, FiWord P1_C, FiClos 
--:         	P2_mustRemove_QMARK_)
--: {
--:         FiBool T3, T4;
--:         FiWord T0, T1, T2_next;
--:         fiPTR_IS_NIL(T3, FiBool, (FiPtr) P0_S);
--:         if (T3) goto L0;
--:         T2_next = P0_S;
--: L2:	fiPTR_IS_NIL(T4, FiBool, (FiPtr) P0_S);
--:         if (T4) goto L3;
--:         if ((FiBool) fiCCall2(FiWord, P2_mustRemove_QMARK_, ((PFmt7) P0_S)->
--:         	X0_elt, P1_C)) goto L4;
--:         P0_S = ((PFmt7) P0_S)->X1_next;
--:         goto L2;
--: L4:	T1 = ((PFmt7) P0_S)->X1_next;
--:         fiFree((FiPtr) P0_S);
--:         P0_S = T1;
--:         goto L2;
--: L3:	T0 = T2_next;
--: L1:	return T0;
--: L0:	T0 = P1_C;
--:         goto L1;
--: }


#include "aldor"
#include "aldorio"

macro T == MachineInteger;

Calix4ti2Monomials: with == add {
	Rep == Record(elt: T, next: %);
	import from Rep;
	dummyCons(x: %): % == cons(nil$Pointer pretend T, x);
	disposeHead!(t: %): % == {-- first t = nil !!!
		u := rest t;
		dispose! rep t; -- dispose the first record
		u;
	}
	macro CODE == {
		R := V := dummyCons S; -- NOTE that R is a PARAMETER.
		while not empty? S repeat {
			if mustRemove?(first S, C) then {
				setRest!(R, S := disposeHead! S);
			} else {
				S := rest(R := S);
			}
		}
		setRest!(R, C); -- append C
		disposeHead! V;
	}

	fOkay(R: %, C: %, mustRemove?: (T, %) -> Boolean): % == {
		empty? R => C;
		S := R; ---------------- Additional line (S is local).
		CODE;
	}
	fWrong(S: %, C: %, mustRemove?: (T, %) -> Boolean): % == {
		empty? S => C;
		CODE;
	}
-------------------------------------------------------------------
	-- The code here is a copy from sal_list.as
	cons(x: T, l: %): % == per [x, l];
	first(l:%):T == {
		import from Boolean;
		assert(~empty? l);
		rep(l).elt;
	}
	rest(l:%):% == {
		import from Boolean;
		assert(~empty? l);
		rep(l).next;
	}
	empty: % == (nil$Pointer) pretend %;
	empty?(l: %): Boolean == nil?(l pretend Pointer)$Pointer;
	setRest!(l:%, s:%):% == {
		import from Boolean;
		assert(~empty? l);
		rep(l).next := s;
	}
}
