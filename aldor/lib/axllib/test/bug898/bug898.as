--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Nov  2 13:02:19 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA21773; Wed, 2 Nov 1994 13:02:19 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2457; Wed, 02 Nov 94 13:02:25 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.1575.Nov.02.13:02:25.-0500>
--*           for asbugs@watson; Wed, 02 Nov 94 13:02:25 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Wed, 02 Nov 94 13:02:24 EST
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id SAA29815 for <asbugs@watson.ibm.com>; Wed, 2 Nov 1994 18:25:39 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id SAA09459 for asbugs@watson.ibm.com; Wed, 2 Nov 1994 18:25:39 +0100
--* Date: Wed, 2 Nov 1994 18:25:39 +0100
--* Message-Id: <199411021725.SAA09459@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [2] cannot pass '%' to another type constructor

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -M2 percent.as
-- Version: 0.37.0
-- Original bug file name: percent.as

-------------------------------- percent.as ---------------------------------
--
-- % asharp -M2 percent.as
-- "percent.as", line 20:         import from MyCatOps %;
--                        .............................^
-- [L11 C30] #1 (Error) Argument 1 of `MyCatOps' did not match any possible
--                      parameter type.
--     The rejected type is Type.
--     Expected type MyCat.

#include "axllib"

MyCat:Category == with { foo: % -> % };

MyCatOps(S:MyCat): with { bar: S -> S } == add { bar(x:S):S == x };

-- The '%' in the import statement has type 'Type' rather than 'MyCat'
-- so this type won't compile.
MyType:MyCat == add {
	import from MyCatOps %;

	foo(x:%):% == bar x;
}

