--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Thu May  4 12:29:23 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA22367; Thu, 4 May 1995 12:29:23 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 8415; Thu, 04 May 95 12:29:23 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.4274.May.04.12:29:19.-0400>
--*           for asbugs@watson; Thu, 04 May 95 12:29:22 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Thu, 04 May 95 12:29:19 EDT
--* Received: from mendel.inf.ethz.ch (mendel.inf.ethz.ch [129.132.12.20]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id SAA24216 for <asbugs@watson.ibm.com>; Thu, 4 May 1995 18:29:15 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by mendel.inf.ethz.ch (8.6.10/8.6.10) id SAA00338 for asbugs@watson.ibm.com; Thu, 4 May 1995 18:24:51 +0200
--* Date: Thu, 4 May 1995 18:24:51 +0200
--* Message-Id: <199505041624.SAA00338@mendel.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [1] Bus error on using BinaryPowering [binpow.as][1.1]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

----------------------------- binpow.as ----------------------------------
--
-- This is a worse instance of bug 928, since the workaround does not work:
-- % axiomxl -Fx binpow.as
-- % binpow
-- Bus error
--

#include "axllib.as"

macro Z == Integer;

FooCat(R:Ring):Category == Ring with {
	coerce: R -> %;
	times!: (%, %) -> %;
	default {
		-- the next line is supposed to be a workaround for bug 928
		prod(a:%, b:%):% == times!(a, b);

		(a:%)^(n:Z):% == {
			import from BinaryPowering(%, prod, Z);
			zero? n => 1;
			u:% := 1;
			power(u, a, n);
		}
	}
}

Foo(R:Ring):FooCat R == R add {
	macro Rep == R;

	times!(a:%, b:%):% == a * b;
	coerce(a:R):% == per a;
}

import from Z, Foo Z;

x := 3::Foo Z;
x^2;

