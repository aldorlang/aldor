--* From IGLIO%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Thu Aug 11 18:16:28 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA19720; Thu, 11 Aug 1994 18:16:28 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 7377; Thu, 11 Aug 94 18:16:32 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.IGLIO.NOTE.VAGENT2.5113.Aug.11.18:16:31.-0400>
--*           for asbugs@watson; Thu, 11 Aug 94 18:16:32 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 5109; Thu, 11 Aug 1994 18:16:31 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Thu, 11 Aug 94 18:16:30 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/4.03)
--*           id AA17188; Thu, 11 Aug 1994 18:11:41 -0400
--* Date: Thu, 11 Aug 1994 18:11:41 -0400
--* From: iglio@watson.ibm.com
--* Message-Id: <9408112211.AA17188@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [3][interactive] 2^(-3) -> the interpreter terminate

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -gloop
-- Version: 0.36.2
-- Original bug file name: /u/iglio/asharp/version/current/tmp/foo.as

-- PI: The problem is probably in the bigint.c package that exit if the power
-- is negative.


#include "axllib"

import from Integer;
import from GeneralAssert;
assertFail((): () +-> 2^(-3));
