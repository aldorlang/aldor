--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Mon Aug 22 16:33:59 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA22257; Mon, 22 Aug 1994 16:33:59 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2641; Mon, 22 Aug 94 16:34:03 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.PETEB.NOTE.VAGENT2.2723.Aug.22.16:34:02.-0400>
--*           for asbugs@watson; Mon, 22 Aug 94 16:34:03 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 2717; Mon, 22 Aug 1994 16:34:02 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Mon, 22 Aug 94 16:34:02 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA06381; Mon, 22 Aug 1994 16:33:56 -0400
--* Date: Mon, 22 Aug 1994 16:33:56 -0400
--* From: peteb@watson.ibm.com (Peter A. Broadbery)
--* Message-Id: <9408222033.AA06381@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [5][tinfer/abnorm/other=notreallyabugatalle] 'with { (a,b,c): % }' should be handled, or raise an error

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 0.36.5
-- Original bug file name: illegal.as

#include "axllib"

Foo: Category == with { (a,b,c,d): %};

-- loses info on b,c,d...
A: Foo == add {
	a: % == 1$Integer pretend %
};

