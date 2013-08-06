--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA04020; Mon, 11 Sep 95 08:12:13 BST
--* Received: from server1.rz.uni-leipzig.de by sun3.nsfnet-relay.ac.uk 
--*           with Internet SMTP id <g.06663-0@sun3.nsfnet-relay.ac.uk>;
--*           Mon, 11 Sep 1995 08:08:23 +0100
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de 
--*           with SMTP (1.37.109.16/16.2) id AA241403300;
--*           Mon, 11 Sep 1995 09:08:20 +0200
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS) 
--*           id AA21574; Sun, 10 Sep 1995 13:12:29 +0100
--* Date: Sun, 10 Sep 1995 13:12:29 +0100
--* From: hemmecke@de.uni-leipzig.informatik.aix550 (Ralf Hemmecke)
--* Message-Id: <9509101212.AA21574@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@uk.co.nag
--* Subject: [7] interactive/non-interactive mode and == and :=
--* Sender: hemmecke@de.uni-leipzig.informatik.aix550

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun xxx.as
-- Version: AXIOM-XL version 1.1.3 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 10-SEP-95
-- AXIOM-XL version 1.1.3 for AIX RS/6000 
-- Subject: interactive/non-interactive mode and == and :=

-- I just found another example where one gets different results in
-- interactive resp. non-interactive mode of axiomxl.

-- Use the program like follows:
-- (1) axiomxl -gloop
--     drag&drop the program into the axiomxl-session.
-- (2) axiomxl -gloop
--     %1>> #include "xxx.as"
-- (3) axiomxl -grun xxx.as

-- Like in a bug submitted yesterday (bug1014), (1) works just fine. 
-- It results in the output:
--! 3,3
--!  () @ TextWriter

-- Whereas (2) yields:
--! 3,0
--!  () @ TextWriter

-- and even worse from (3) I got:
--! 3,537260240

-- If this is not a bug, would you please explain that different behaviour.

---- BEGIN xxx.as ---------
#include "axllib"
import from List Integer;
li:List Integer  := [1,1,1];
n:SingleInteger := #li;
m:SingleInteger == n;
print << n<< "," << m << newline;
---- END xxx.as ---------
