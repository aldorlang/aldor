--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Oct 28 10:43:48 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA21662; Fri, 28 Oct 1994 10:43:48 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 1689; Fri, 28 Oct 94 10:43:54 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.4039.Oct.28.10:43:54.-0400>
--*           for asbugs@watson; Fri, 28 Oct 94 10:43:54 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Fri, 28 Oct 94 10:43:53 EDT
--* Received: from vinci.inf.ethz.ch (bronstei@vinci.inf.ethz.ch [129.132.12.46]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id PAA16099 for <asbugs@watson.ibm.com>; Fri, 28 Oct 1994 15:43:45 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id PAA24651 for asbugs@watson.ibm.com; Fri, 28 Oct 1994 15:43:44 +0100
--* Date: Fri, 28 Oct 1994 15:43:44 +0100
--* Message-Id: <199410281443.PAA24651@vinci.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [2] Tuple Type is unusable at runtime

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -Fx foo.as
-- Version: 0.37.0
-- Original bug file name: foo.as

------------------------------- foo.as ----------------------------------
-- It looks like "Tuple Type" is ok at compile-time, but provokes problems
-- at runtime:
--
-- % asharp -Fx foo.as
-- % foo
-- Looking in Foo(T==<value>) for foo with code 905550213
-- Export not found

#include "axllib"

macro Z == SingleInteger;

Foo(T:Tuple Type): with {
	foo: () -> %;
} == add {
	macro Rep == Z;
	import from Rep;
	foo():%	== per 1;
}

m:Foo(Z) := foo();

n:Foo(Z,Z) := foo();


