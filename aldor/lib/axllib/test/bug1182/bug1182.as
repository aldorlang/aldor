--* From Manuel.Bronstein@sophia.inria.fr  Thu Nov 18 17:00:54 1999
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id RAA14235
--* 	for <ax-bugs@nag.co.uk>; Thu, 18 Nov 1999 17:00:49 GMT
--* Received: by droopix.inria.fr (8.8.8/8.8.5) id SAA29087 for ax-bugs@nag.co.uk; Thu, 18 Nov 1999 18:00:23 +0100
--* Date: Thu, 18 Nov 1999 18:00:23 +0100
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199911181700.SAA29087@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] bad optimization of machine int's!

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -dSALLI -fc -q3 slow.as
-- Version: 1.1.12p2
-- Original bug file name: slow.as

-------------------------------- slow.as ------------------------------
--
-- YOU NEED SALLI 0.1.12b IN ORDER TO REPRODUCE THIS BUG
--   (available from http://www.inria.fr/cafe/Manuel.Bronstein/salli.html)
--
-- Although salli's 'MachineInteger' and 'PrimitiveArray' are defined to be
-- SInt$Machine and Arr$Machine like axllib's 'SingleInteger' and
-- 'PrimitiveArray', the optimizer treats both very differently,
-- and the following program turns out to be 15 slower in salli than axllib
-- because of poor optimization.
--
-- Here is the code generated on top of axllib:
-- static FiWord
-- CF1_sum(FiEnv e1, FiWord P0_a, FiWord P1_n)
-- {
--         FiSInt T0_i, T1_s, T3;
--         FiWord T2;
--         T1_s = 0;
--         T0_i = 1;
-- L0:     if ((FiSInt) P1_n < T0_i) goto L1;
--         T3 = T0_i - 1;
--         T2 = ((FiWord*) P0_a)[T3];
--         T1_s = T1_s + (FiSInt) T2;
--         T0_i = T0_i + 1;
--         goto L0;
-- L1:     return (FiWord) T1_s;
-- }
--
-- Here is the code generated on top of salli:
-- static FiWord
-- CF1_sum(FiEnv e1, FiWord P0_a, FiWord P1_n)
-- {
--         FiEnv T4;
--         FiWord T3;
--         FiSInt T0_maxindex, T1_i, T2_s;
--         PFmt5 l1;
--         l1 = (PFmt5) fiEnvLevel(e1);
--         T2_s = 0;
--         T1_i = 0;
--         T0_maxindex = (FiSInt) P1_n - 1;
-- L0:     if (!(FiBool) fiCCall2(FiWord, l1->X3__LT__EQ_, (FiWord) T1_i, (FiWord)
--                 T0_maxindex)) goto L1;
--         fiEnvEnsure(l1->X2_apply->env);
--         T4 = l1->X2_apply->env;
--         T3 = fiCCall2(FiWord, ((PFmt6) fiEnvLevel0(T4))->X12_elt, (
--                 FiHInt*) P0_a, T1_i);
--         T2_s = T2_s + (FiSInt) T3;
--         T1_i = T1_i + 1;
--         goto L0;
-- L1:     return (FiWord) T2_s;
-- }
--
-- The definition of apply in axllib is
--             apply(a: %, i: SI): S == get(S)(rep a, (i-1)::BSInt)
-- The definition of apply in salli is
--         local elt:(Arr, SInt) -> S      == get S;
--         apply(x:%, n:SI):S              == elt(rep x, n::SInt);
-- Why doesn't it get inlined?
-- Is 'SingleInteger' a special type name that the optimizer looks for?
--


#if SALLI
#include "salli"
macro I == MachineInteger;
macro firstindex == 0;
#else
#include "axllib"
macro I == SingleInteger;
macro firstindex == 1;
#endif

macro A == PrimitiveArray;

-- sum the first n elements of a (in a stupid way, this is to compare C codes)
local sum(a:A I, n:I):I == {
	s := 0;
	i := firstindex;
	maxindex := i + prev n;
	while i <= maxindex repeat {
		s := s + a.i;
		i := next i;
	}
	s;
}

