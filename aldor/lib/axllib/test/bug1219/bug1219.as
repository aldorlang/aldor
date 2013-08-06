--* From Manuel.Bronstein@sophia.inria.fr  Mon May 29 12:44:31 2000
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id MAA20422
--* 	for <ax-bugs@nag.co.uk>; Mon, 29 May 2000 12:44:30 +0100 (BST)
--* Received: by droopix.inria.fr (8.10.0/8.10.0) id e4TBiR116833 for ax-bugs@nag.co.uk; Mon, 29 May 2000 13:44:27 +0200
--* Date: Mon, 29 May 2000 13:44:27 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200005291144.e4TBiR116833@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] cat prevent basic inlining

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q3 -fc noopt.as
-- Version: 1.1.12p4
-- Original bug file name: noopt.as

-------------------------------- noopt.as ------------------------------
--
-- The array access a.i in sum() is not inlined (fiCCall2 instead):
--
-- static FiWord
-- CF23_sum(FiEnv e1, FiWord P0_a, FiWord P1_n)
-- {
--         FiWord T2;
--         FiSInt T0_i, T1_s;
--         PFmt15 l1;
--         l1 = (PFmt15) fiEnvLevel(e1);
--         T1_s = 0L;
--         T0_i = 1L;
-- L0:     if ((FiSInt) P1_n < T0_i) goto L1;
--         T2 = fiCCall2(FiWord, l1->X4_apply, P0_a, (FiWord) T0_i);
--         T1_s = T1_s + (FiSInt) T2;
--         T0_i = T0_i + 1L;
--         goto L0;
-- L1:     return (FiWord) T1_s;
-- }
--

#include "axllib"

macro A == PArray;
macro Z == SingleInteger;

-- This category seems to prevent apply$PArray to be inlined
define Foo(T:BasicType):Category == FiniteLinearAggregate T with { new: Z->% };

PArray(T:BasicType): Foo T == add {
	import from Machine;
	macro Rep == Arr;

	-- The 2 functions below do not get inlined in clients
	-- except if Foo is removed and PArray is declared to be
	-- FiniteLinearAggregate T with { new: Z -> % } == add {...}
	apply(x:%, n:Z):T		== get(T)(rep x, n::SInt);
	set!(x:%, n:Z, y:T):T		== set!(T)(rep x, n::SInt, y);

	new(n:Z):%			== per(array(Z)(0, n::SInt));
	empty():%			== (nil$Pointer) pretend %;
	empty?(a:%):Boolean		== nil?(a pretend Pointer)$Pointer;
	bracket(g:Generator T):%	== empty();
	bracket(t:Tuple T):%		== empty();
	#(x:%):Z			== 0;
	(x:%) = (y:%):Boolean		== false;
	sample:%			== empty();
	generator(x:%):Generator T	== generate {};
	(p:TextWriter) << (x:%):TextWriter == p;
}


-- sum the first n elements of a (in a stupid way, this is to check the C code)
sum(a:A Z, n:Z):Z == {
	s := 0;
	i := 1;
	maxindex := n;
	while i <= maxindex repeat {
		s := s + a.i;
		i := next i;
	}
	s;
}

