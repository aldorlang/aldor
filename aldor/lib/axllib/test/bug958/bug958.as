--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Mar 14 12:06:29 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA01569; Tue, 14 Mar 1995 12:06:29 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 9547; Tue, 14 Mar 95 12:05:38 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.7179.Mar.14.12:05:32.-0500>
--*           for asbugs@watson; Tue, 14 Mar 95 12:05:38 -0500
--* Received: from bernina.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Tue, 14 Mar 95 12:05:32 EST
--* Received: from vinci.inf.ethz.ch by bernina.ethz.ch with SMTP inbound;
--*           Tue, 14 Mar 1995 18:05:08 +0100
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id SAA16258
--*           for asbugs@watson.ibm.com; Tue, 14 Mar 1995 18:05:07 +0100
--* Date: Tue, 14 Mar 1995 18:05:07 +0100
--* From: bronstei <bronstei@inf.ethz.ch>
--* Message-Id: <199503141705.SAA16258@vinci.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [4] Default argument don't work with apply [defbug.as][1.0.6]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

------------------------------- defbug.as --------------------------
--
-- Default value for arguments do *not* work for the apply function:
-- % axiomxl -Fx -Q0 defbug.as
-- % defbug
-- 10
-- 389274
--

#include "axllib.as"

macro Z == SingleInteger;
import from Z;

Foo: with {
	foo: (n:Z, a:Z, b:Z == 0) -> Z;
	apply: (n:Z, a:Z, b:Z == 0) -> Z;
} == add {
	apply(n:Z, a:Z, b:Z == 0):Z == foo(n, a, b);
	foo(n:Z, a:Z, b:Z == 0):Z == (n * a) + b;
}

import from Foo;

x:Z := 5;
y:Z := 2;
-- both statements should print 10
print << foo(x, y) << newline;
print << x(y) << newline;

