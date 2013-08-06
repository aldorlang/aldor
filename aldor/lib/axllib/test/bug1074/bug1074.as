--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA20943; Mon, 29 Apr 96 23:44:08 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id SAA19952; Mon, 29 Apr 1996 18:40:35 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA40802; Mon, 29 Apr 1996 18:38:46 -0400
--* Date: Mon, 29 Apr 1996 18:38:46 -0400
--* Message-Id: <199604292238.AA40802@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [8] Second result from scan$Format

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.5.x
-- Original bug file name: /dev/null

--+ It is unclear what the second result of `scan' in the `Format' package
--+ is, but whatever it is it gets it wrong!
--+ 
--+ A quick perusal of the code leads me to believe that it is *meant* to
--+ return the starting offset of the next number in the input string, but
--+ it *actually* appears to return the input offset plus one. Maybe
--+ someone thought that `sscanf' returned the number of characters read
