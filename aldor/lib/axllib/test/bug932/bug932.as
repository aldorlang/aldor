--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Mon Dec 19 12:49:05 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17900; Mon, 19 Dec 1994 12:49:05 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2855; Mon, 19 Dec 94 12:49:03 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.1249.Dec.19.12:49:02.-0500>
--*           for asbugs@watson; Mon, 19 Dec 94 12:49:03 -0500
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Mon, 19 Dec 94 12:49:02 EST
--* Received: from vinci.inf.ethz.ch (bronstei@vinci.inf.ethz.ch [129.132.12.46]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id SAA06888 for <asbugs@watson.ibm.com>; Mon, 19 Dec 1994 18:49:02 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id SAA20049 for asbugs@watson.ibm.com; Mon, 19 Dec 1994 18:48:58 +0100
--* Date: Mon, 19 Dec 1994 18:48:58 +0100
--* Message-Id: <199412191748.SAA20049@vinci.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [4] pretend needed on type variable

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -M2 condcat.as
-- Version: 1.0.3
-- Original bug file name: condcat.as

------------------------ condcat.as ------------------------
-- asharp -M2 condcat.as
-- "condcat.as", line 16:        if T has Group then MyCat1 T;
--                       ...................................^
-- [L9 C36] #1 (Error) Argument 1 of `MyCat1' did not match any possible parameter type.
--     The rejected type is BasicType.
--     Expected type Group.

#include "axllib"

MyCat1(T:Group):Category == with { foo: % -> % };

MyCat2(T:BasicType): Category == with {
	bar: % -> %;
	-- this needs a (T pretend Group) to compile
	if T has Group then MyCat1 T;
}

