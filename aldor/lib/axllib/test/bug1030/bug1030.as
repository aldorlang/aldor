--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA04768; Tue, 26 Sep 95 11:26:45 BST
--* Received: from server1.rz.uni-leipzig.de by sun2.nsfnet-relay.ac.uk 
--*           with Internet SMTP id <sg.14492-0@sun2.nsfnet-relay.ac.uk>;
--*           Tue, 26 Sep 1995 11:22:39 +0100
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de 
--*           with SMTP (1.37.109.16/16.2) id AA273980842;
--*           Tue, 26 Sep 1995 11:20:43 +0100
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS) 
--*           id AA29894; Tue, 26 Sep 1995 11:23:12 +0100
--* Date: Tue, 26 Sep 1995 11:23:12 +0100
--* From: hemmecke@de.uni-leipzig.informatik.aix550 (Ralf Hemmecke)
--* Message-Id: <9509261023.AA29894@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@uk.co.nag
--* Subject: [8] function returning a domain
--* Sender: hemmecke@de.uni-leipzig.informatik.aix550

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DCW1 -DCP1 -DCA0 xxx.as
-- Version: AXIOM-XL version 1.1.3 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 26-SEP-95
-- AXIOM-XL version 1.1.3 for AIX RS/6000
-- Subject: function returning a domain

-- Compile this with 
--     axiomxl -DWx -DPy -DAz xxx.as
-- where x,y,z \in {0,1}. This gives 8 possibilities.
-- Axiom-XL gives no message in either case but with

-- axiomxl -DCW1 -DCP1 -DCA0 xxx.as 

#assert CW1
#assert CP1
#assert CA0

-- the compiler process will be killed. The output is:
--:Killed

-- Looking at the resources during the run of the compiler one can see that
-- it uses steadily growing memory and is killed when it reaches about
-- 40000KB memory size.

-- One could also have a line 
-- import from Integer;
-- instead of the line with the print command.

-- terms looks nearly like a domain constructor, but the intention was to
-- have a function which according to its parameters returned the 
-- corresponding domain out of some available ones.
 
#include "axllib"

macro {
#if CW0
  CTCX == BasicType;
#elseif CW1
  CTCX == BasicType with {};
#endif
}

terms(i:Integer): CTCX == {
#if CP1
  if odd? i then print << "This is odd." << newline;
#endif
#if CA0
  Integer;
#elseif CA1
  Integer add {};
#endif
}
