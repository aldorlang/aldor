--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Jun 21 17:21:23 1995
--* Received: from yktvmv-ob.watson.ibm.com by asharp.watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17107; Wed, 21 Jun 1995 17:21:23 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0841; Wed, 21 Jun 95 17:21:22 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.PETERB.NOTE.YKTVMV.7375.Jun.21.17:21:21.-0400>
--*           for asbugs@watson; Wed, 21 Jun 95 17:21:22 -0400
--* Received: from sun2.nsfnet-relay.ac.uk by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Wed, 21 Jun 95 17:21:20 EDT
--* Via: uk.co.iec; Wed, 21 Jun 1995 22:21:03 +0100
--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1) id AA11229;
--*           Wed, 21 Jun 95 22:23:50 BST
--* From: Peter Broadbery <peterb@num-alg-grp.co.uk>
--* Date: Wed, 21 Jun 95 22:21:25 +0100
--* Message-Id: <23305.9506212121@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/NAg-1.0) id AA23305;
--*           Wed, 21 Jun 95 22:21:25 +0100
--* To: asbugs@watson.ibm.com
--* Subject: [3] Can't have Union with same-typed members

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib"

import from Union(x: Integer, y: Integer);

print << ([x==3] case x) << newline;
print << ([x==3] case y) << newline;
print << ([y==3] case x) << newline;
print << ([y==3] case y) << newline;


