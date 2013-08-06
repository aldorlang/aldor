--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Nov 16 10:05:18 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA19953; Wed, 16 Nov 1994 10:05:18 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 9917; Wed, 16 Nov 94 10:05:24 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.4503.Nov.16.10:05:23.-0500>
--*           for asbugs@watson; Wed, 16 Nov 94 10:05:24 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Wed, 16 Nov 94 10:05:22 EST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id QAA18780 for <asbugs@watson.ibm.com>; Wed, 16 Nov 1994 16:05:14 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id QAA19014 for asbugs@watson.ibm.com; Wed, 16 Nov 1994 16:05:13 +0100
--* Date: Wed, 16 Nov 1994 16:05:13 +0100
--* Message-Id: <199411161505.QAA19014@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [7] Pretend required where it shouldn't be

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -M2 cycle.as
-- Version: 0.37.0
-- Original bug file name: cycle.as

------------------------------- cycle.as ----------------------------------
-- When compiling MyRec, List(%) is not accepted where List(MyRec) is wanted:
--
-- % asharp -M2 cycle.as
-- "cycle.as", line 40:
--         bar(r:%):Z              == bar(rep(r).val)$tyype(r);
-- .............................................^
-- [L29 C46] #1 (Error) Argument 1 of `bar$tyype(r)' did not match any possible
-- parameter type.
--     The rejected type is val: List(%).
--     Expected type List(MyRec).
--

#include "axllib"

macro Z	== SingleInteger;

MyCat: Category == with {
	foo: List MyRec -> Z;
	bar: List MyRec -> Z;
}

MyType: MyCat == add {
	foo(l:List MyRec):Z == #l;
	bar(l:List MyRec):Z == #l;
}

MyRec: with {
	foo: % -> Z;
	bar: % -> Z;
} == add {
	macro Rep == Record(typ:MyCat, val:List %);

	import from Rep;

	tyype(r:%):MyCat	== rep(r).typ;

-- foo compiles, but not bar, so the pretend is currently necessary:
	foo(r:%):Z		== foo(rep(r).val pretend List MyRec)$tyype(r);
	bar(r:%):Z		== bar(rep(r).val)$tyype(r);
}
