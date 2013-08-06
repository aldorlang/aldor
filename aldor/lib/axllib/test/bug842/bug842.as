--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 30 14:27:03 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA21332; Tue, 30 Aug 1994 14:27:03 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 4739; Tue, 30 Aug 94 14:27:07 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.LAURENT.NOTE.YKTVMV.0371.Aug.30.14:27:06.-0400>
--*           for asbugs@watson; Tue, 30 Aug 94 14:27:07 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Tue, 30 Aug 94 14:27:05 EDT
--* Received: from ru7.inf.ethz.ch (ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.9/8.6.9) with ESMTP id UAA25272 for <asbugs@watson.ibm.com>; Tue, 30 Aug 1994 20:26:58 +0200
--* Received: (bernardi@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id UAA17188 for asbugs@watson.ibm.com; Tue, 30 Aug 1994 20:26:54 +0200
--* Date: Tue, 30 Aug 1994 20:26:54 +0200
--* From: "Laurent Bernardin" <Laurent.Bernardin@inf.ethz.ch>
--* Precedence: first-class
--* Message-Id: <199408301826.UAA17188@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [2] export not found at runtime [modp1.as][0.36.0]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

----------------------- modp1.as -----------------------
-- asharp -M2 -Fx modp1.as
-- ru7[bernardi]: asharp> modp1
-- Looking in PM for bug with code 51495535
-- Export not found

#include "axllib"

macro {
	SI == SingleInteger;
}

PM: with {
	bug:%;
} ==  add {
	DT ==> PrimitiveArray SI;
	Rep ==> Record(size: SI, data: DT);

	import from DT,SI,Rep;
	
	-- all ok if this line is commented out:
	foo:% == bug;

	bug:% == per [1,new(1,1)];
		
}

import from PM;

a:PM := bug;
