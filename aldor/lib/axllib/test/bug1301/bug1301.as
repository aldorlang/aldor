--* From youssef@buphyk.bu.edu  Wed Mar 28 20:35:34 2001
--* Received: from server-18.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id UAA07572
--* 	for <ax-bugs@nag.co.uk>; Wed, 28 Mar 2001 20:35:33 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 15441 invoked from network); 28 Mar 2001 19:33:46 -0000
--* Received: from buphyk.bu.edu (128.197.41.10)
--*   by server-18.tower-4.starlabs.net with SMTP; 28 Mar 2001 19:33:46 -0000
--* Received: (from youssef@localhost)
--* 	by buphyk.bu.edu (8.9.3/8.9.3) id OAA10103
--* 	for ax-bugs@nag.co.uk; Wed, 28 Mar 2001 14:34:57 -0500
--* Date: Wed, 28 Mar 2001 14:34:57 -0500
--* From: Saul Youssef <youssef@buphyk.bu.edu>
--* Message-Id: <200103281934.OAA10103@buphyk.bu.edu>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] bug in SingleInteger

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: bug.as

--+ --
--+ --  Martin,  There is a bug in SingleInteger.  axiomxl -g interp of this file
--+ --  results in 
--+ --
--+ --  4
--+ --  4
--+ --  0
--+ --  0
--+ --
--+ --  which is wrong.  /\ is supposed to be the greatest lower bound resulting
--+ --  in
--+ --
--+ --  4
--+ --  4
--+ --  2
--+ --  2
--+ --     Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ import from SingleInteger
--+ 
--+ print << (4 /\ 7) << newline
--+ print << (7 /\ 4) << newline
--+ print << (2 /\ 5) << newline
--+ print << (5 /\ 2) << newline
--+ 
--
--  Martin,  There is a bug in SingleInteger.  axiomxl -g interp of this file
--  results in 
--
--  4
--  4
--  0
--  0
--
--  which is wrong.  /\ is supposed to be the greatest lower bound resulting
--  in
--
--  4
--  4
--  2
--  2
--     Saul
--
#include "axllib"
#pile

import from SingleInteger

print << (4 /\ 7) << newline
print << (7 /\ 4) << newline
print << (2 /\ 5) << newline
print << (5 /\ 2) << newline


