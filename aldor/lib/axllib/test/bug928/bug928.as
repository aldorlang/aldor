--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Sun Dec 18 08:29:47 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA23874; Sun, 18 Dec 1994 08:29:47 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 9625; Sun, 18 Dec 94 08:29:45 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.9129.Dec.18.08:29:45.-0500>
--*           for asbugs@watson; Sun, 18 Dec 94 08:29:45 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Sun, 18 Dec 94 08:29:45 EST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id OAA08293 for <asbugs@watson.ibm.com>; Sun, 18 Dec 1994 14:29:45 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id OAA08093 for asbugs@watson.ibm.com; Sun, 18 Dec 1994 14:29:44 +0100
--* Date: Sun, 18 Dec 1994 14:29:44 +0100
--* Message-Id: <199412181329.OAA08093@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [1] runtime seg fault

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -Q0 -Fx spf.as
-- Version: 1.0.3
-- Original bug file name: spf.as

----------------------------- spf.as ----------------------------------
-- This code generation bug has already been partially fixed, but
-- is still showing up:
-- % asharp -Q0 -Fx spf.as
-- % spf
-- Segmentation fault

#include "axllib"

macro Z == SingleInteger;

SmallPrimeField(p:Z):FiniteField == SingleIntegerMod p add {
	macro Rep == SingleIntegerMod p;

	import from Rep;

	#:Integer	== p::Integer;
	reduce(a:Z):%	== per(a::Rep);
	inv(a:%):%	== mod_/(1, lift a, p)::%;

	(a:%)^(n:Integer):% == {
		import from BinaryPowering(%, *, Z);
		m:Z := lift(n::%);
		n < 0 => power(1, inv a, m);
		power(1, a, m);
	}
}

import from Z, Integer, SmallPrimeField 11;

x:SmallPrimeField 11 := reduce 2;
x^2;
