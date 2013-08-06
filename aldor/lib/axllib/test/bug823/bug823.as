--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 23 10:38:43 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA18964; Tue, 23 Aug 1994 10:38:43 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 7335; Tue, 23 Aug 94 10:38:46 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.TEKE.NOTE.YKTVMV.9935.Aug.23.10:38:44.-0400>
--*           for asbugs@watson; Tue, 23 Aug 94 10:38:45 -0400
--* Received: from piger.matematik.su.se by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Tue, 23 Aug 94 10:38:44 EDT
--* Received: by piger.matematik.su.se (AIX 3.2/UCB 5.64/4.03)
--*           id AA10134; Tue, 23 Aug 1994 16:22:48 -0500
--* Date: Tue, 23 Aug 1994 16:22:48 -0500
--* From: teke@piger.matematik.su.se (Torsten Ekedahl)
--* Message-Id: <9408232122.AA10134@piger.matematik.su.se>
--* To: asbugs@watson.ibm.com
--* Subject: [5] Signature not conditionally added.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: none
-- Version: 0.36.5
-- Original bug file name: hest.as

--+ A signature is not added in the yes-branch of a query for a signature.
--+
#include "axllib"


hh ==
 add {
   rr : String :=
     if % has with { name : String } then name; else "hh";
};




