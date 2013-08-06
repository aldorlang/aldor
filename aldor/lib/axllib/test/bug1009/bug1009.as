--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA09574; Wed, 16 Aug 95 21:46:45 BST
--* Received: from watson.ibm.com by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.23494-0@sun3.nsfnet-relay.ac.uk>;
--*           Wed, 16 Aug 1995 21:42:47 +0100
--* Received: from WATSON by watson.ibm.com (IBM VM SMTP V2R3) with BSMTP id 3091;
--*           Wed, 16 Aug 95 16:41:01 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.01 on VAGENT2" 
--*           id 9585; Wed, 16 Aug 1995 16:41:01 EDT
--* Received: from watson.ibm.com 
--*           by yktvmv.watson.ibm.com (IBM VM SMTP V2Rx) with TCP;
--*           Wed, 16 Aug 95 16:41:01 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/930311) id AA26698;
--*           Wed, 16 Aug 1995 16:40:49 -0400
--* Date: Wed, 16 Aug 1995 16:40:49 -0400
--* From: asbugs@com.ibm.watson (S Watt)
--* Message-Id: <9508162040.AA26698@watson.ibm.com>
--* To: ax-bugs@uk.co.nag
--* Subject: forwbug
--* Sender: asbugs@com.ibm.watson

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


--  Reporter:    bronstei@inf.ethz.ch
--  Description: [4] List Cross still has runtime problems [listcross.as][1.1.2]

------------------------------ unknown.as ---------------------------------
--* From @yktvmv.watson.ibm.com:postmaster@watson.vnet.ibm.com  Wed Aug 16 16:40:46 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA26924; Wed, 16 Aug 1995 16:40:46 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 1477; Wed, 16 Aug 95 16:40:56 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.0401.Aug.16.16:40:55.-0400>
--*           for asbugs@watson; Wed, 16 Aug 95 16:40:56 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Wed, 16 Aug 95 16:40:55 EDT
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id WAA29786 for <asbugs@watson.ibm.com>; Wed, 16 Aug 1995 22:40:35 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id WAA14550 for asbugs@watson.ibm.com; Wed, 16 Aug 1995 22:40:33 +0200
--* Date: Wed, 16 Aug 1995 22:40:33 +0200
--* Message-Id: <199508162040.WAA14550@ru7.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [4] List Cross still has runtime problems [listcross.as][1.1.2]

--------------------------------- listcross.as --------------------------------
--
-- Looks like List Cross is not at 100% yet:
--
-- % axiomxl -Fx listcross.as
-- % listcross
-- Looking in List(Cross((Integer, Integer))) for cons with code 745483955
-- Export not found
--

#include "axllib.as"

macro RZ == Cross(R, Integer);

-- The bug disappears if the following - unused - type is removed!
Bar(R:Monoid, L:ListCategory List R): with { foo: L -> List RZ } == add {
	foo(f:L):List RZ == empty();
}
			
Foo(R:Ring, L:ListCategory R): with { foo: L -> List RZ } == add {
	foo(l:L):List RZ == {
		import from Integer, RZ;
		cons((first l, 1), empty());
	}
}			

main():List Cross(Integer, Integer) == {
	import from Integer, List Integer, Foo(Integer, List Integer);
	foo [1];
}

main();
