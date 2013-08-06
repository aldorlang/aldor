--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA08722; Wed, 16 Aug 95 16:22:46 BST
--* From: Peter Broadbery <peterb@uk.co.nag>
--* Date: Wed, 16 Aug 95 16:19:14 +0100
--* Message-Id: <2945.9508161519@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/NAg-1.0)
--* 	id AA02945; Wed, 16 Aug 95 16:19:14 +0100
--* Apparently-To: ax-bugs@nag.co.uk
--* Apparently-To: fixbug@nag.co.uk

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA01694; Thu, 10 Aug 95 18:21:19 BST
--* Received: from watson.ibm.com by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.08433-0@sun3.nsfnet-relay.ac.uk>;
--*           Thu, 10 Aug 1995 18:17:34 +0100
--* Received: from WATSON by watson.ibm.com (IBM VM SMTP V2R3) with BSMTP id 6899;
--*           Thu, 10 Aug 95 13:12:44 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.01 on VAGENT2" 
--*           id 3511; Thu, 10 Aug 1995 13:12:44 EDT
--* Received: from matteo.watson.ibm.com 
--*           by yktvmv.watson.ibm.com (IBM VM SMTP V2Rx) with TCP;
--*           Thu, 10 Aug 95 13:12:44 EDT
--* Received: by matteo.watson.ibm.com (AIX 3.2/UCB 5.64/920123) id AA18751;
--*           Thu, 10 Aug 1995 13:13:11 -0400
--* Date: Thu, 10 Aug 1995 13:13:11 -0400
--* From: dooley@com.ibm.watson (Sam Dooley)
--* Message-Id: <9508101713.AA18751@matteo.watson.ibm.com>
--* To: ax-bugs@uk.co.nag
--* Subject: [9] Testing ax-bugs address.
--* Sender: dooley@com.ibm.watson

--@ Fixed  by:  PAB   
--@ Tested by:  PAB 
--@ Summary:    PAB 


-- Command line: asharp -Fx test.as
-- Version: r1-0-8-3
-- Original bug file name: test.as

#include "axllib"

print << "Hello, world!" << newline;
 


