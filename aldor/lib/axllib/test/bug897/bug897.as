--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Nov  2 12:52:47 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17249; Wed, 2 Nov 1994 12:52:47 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 1975; Wed, 02 Nov 94 12:52:54 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.0123.Nov.02.12:52:53.-0500>
--*           for asbugs@watson; Wed, 02 Nov 94 12:52:54 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Wed, 02 Nov 94 12:52:52 EST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id SAA01116 for <asbugs@watson.ibm.com>; Wed, 2 Nov 1994 18:52:42 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id SAA09885 for asbugs@watson.ibm.com; Wed, 2 Nov 1994 18:52:41 +0100
--* Date: Wed, 2 Nov 1994 18:52:41 +0100
--* Message-Id: <199411021752.SAA09885@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [5] cannot have only one optional parameter to a type

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -M2 defparam2.as
-- Version: 0.37.0
-- Original bug file name: defparam2.as

----------------------------- defparam2.as ----------------------------
--
-- % asharp -M2 defparam2.as
-- "defparam2.as", line 26: q:MyType() == foo 5;
--                          ..^...........^
-- [L19 C3] #1 (Error) There are no suitable meanings for the operator `MyType'.
--   MyType: (SingleInteger == 1) -> (
--                 BasicType with foo: ..., a local
--       rejected because it cannot take 0 arguments.

#include "axllib"

macro Z == SingleInteger;

MyType(v:Z == 1):BasicType with { foo: Z -> % } == Z add {
	macro Rep == Z;
	import from Rep;

	foo(x:Z):% == per x;
}

import from Z;

-- this fails to compile
-- the problem does not occur if a non-optional argument is added to MyType.
q:MyType() == foo 5;

