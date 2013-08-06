--* From SMWATT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Mon Jun 27 21:31:56 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA24596; Mon, 27 Jun 1994 21:31:56 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2423; Mon, 27 Jun 94 21:31:57 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.SMWATT.NOTE.VAGENT2.4153.Jun.27.21:31:56.-0400>
--*           for asbugs@watson; Mon, 27 Jun 94 21:31:57 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 4147; Mon, 27 Jun 1994 21:31:56 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Mon, 27 Jun 94 21:31:55 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/900524)
--*           id AA15406; Mon, 27 Jun 1994 21:31:33 -0400
--* Date: Mon, 27 Jun 1994 21:31:33 -0400
--* From: smwatt@watson.ibm.com (Stephen Watt)
--* X-External-Networks: yes
--* Message-Id: <9406280131.AA15406@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [3] Message incorrectly beleives no return type is given. [foo.as][v36-epsilon]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib"

f(m: SingleInteger)(n: SingleInteger == 3): SingleInteger == m + n;
