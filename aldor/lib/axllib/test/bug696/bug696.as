--* From SMWATT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Jun 15 21:18:49 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA16238; Wed, 15 Jun 1994 21:18:49 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0889; Wed, 15 Jun 94 21:18:50 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.SMWATT.NOTE.VAGENT2.3819.Jun.15.21:18:50.-0400>
--*           for asbugs@watson; Wed, 15 Jun 94 21:18:50 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 3815; Wed, 15 Jun 1994 21:18:50 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Wed, 15 Jun 94 21:18:49 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA13308; Wed, 15 Jun 1994 21:14:02 -0400
--* Date: Wed, 15 Jun 1994 21:14:02 -0400
--* From: smwatt@watson.ibm.com (Stephen Watt)
--* X-External-Networks: yes
--* Message-Id: <9406160114.AA13308@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [4] Should import from each of the multiple return types [yoyo.as][mine]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib"

foo(): (Integer, Integer) ==
{
	0 = 0 => (1,1);
	(2,2)
}
		
