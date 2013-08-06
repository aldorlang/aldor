--* From DOOLEY%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Aug 19 15:56:22 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA27035; Fri, 19 Aug 1994 15:56:22 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2417; Fri, 19 Aug 94 15:56:26 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.DOOLEY.NOTE.VAGENT2.8913.Aug.19.15:56:25.-0400>
--*           for asbugs@watson; Fri, 19 Aug 94 15:56:26 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 8909; Fri, 19 Aug 1994 15:56:25 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Fri, 19 Aug 94 15:56:25 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA18580; Fri, 19 Aug 1994 15:50:35 -0400
--* Date: Fri, 19 Aug 1994 15:50:35 -0400
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9408191950.AA18580@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [4] Enumeration doesn't implement print.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -gloop < enum.as
-- Version: 0.36.4
-- Original bug file name: enum.as

-- asharp -gloop

#include "axllib"

UU ==> Union(element:Integer,failed:Enumeration(failed));
g:UU := [5];
g.failed;
