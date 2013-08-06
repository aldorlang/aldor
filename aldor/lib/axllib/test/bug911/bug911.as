--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Nov 16 10:18:39 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA16356; Wed, 16 Nov 1994 10:18:39 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0715; Wed, 16 Nov 94 10:18:43 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.7329.Nov.16.10:18:41.-0500>
--*           for asbugs@watson; Wed, 16 Nov 94 10:18:42 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Wed, 16 Nov 94 10:18:40 EST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id QAA19435 for <asbugs@watson.ibm.com>; Wed, 16 Nov 1994 16:18:31 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id QAA19147 for asbugs@watson.ibm.com; Wed, 16 Nov 1994 16:18:25 +0100
--* Date: Wed, 16 Nov 1994 16:18:25 +0100
--* Message-Id: <199411161518.QAA19147@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [4] type is evaluated when it shouldn't

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -Fx tooearly.as
-- Version: 0.37.0
-- Original bug file name: tooearly.as

------------------------------- tooearly.as ----------------------------------
--
-- A type appearing in a function is evaluated even if the flow never
-- reaches the point where the type is used. This causes the following
-- runtime error:
--
-- % asharp -Fx tooearly.as
-- % tooearly
-- User error: Bad union branch.
--

#include "axllib"

macro Z == SingleInteger;

MyCat: Category == with { foo: () -> Z };

MyRec: with {
	myrec: Z -> %;
	foo:   % -> Z;
} == add {
	macro Rep == Union(uint:Z, utyp:MyCat);

	import from Rep;

	myrec(n:Z):%		== per [n];
	int?(t:%):Boolean	== rep(t) case uint;
	int(t:%):Z		== rep(t).uint;
	tyype(t:%):MyCat	== rep(t).utyp;

-- The problem in foo, is that the call to tyype(t) is done at
-- runtime even if int? t returns true!
	foo(t:%):Z == {
		int? t => int t;
		foo()$tyype(t);
	}
}

import from Z, MyRec;

grec := myrec 5;
gn   := foo grec;

