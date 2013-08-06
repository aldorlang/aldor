--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA08132; Fri, 12 Apr 96 15:00:04 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id JAA08108; Fri, 12 Apr 1996 09:56:02 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA11636; Fri, 12 Apr 1996 09:54:25 -0400
--* Date: Fri, 12 Apr 1996 09:54:25 -0400
--* Message-Id: <199604121354.AA11636@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [4] Failure for mixed-case names

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Ginterp bug-CAPITAL.as
-- Version: AXIOM-XL version 1.1.5 for AIX RS/6000
-- Original bug file name: bug-CAPITAL.as

--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    1.8 s  0  1  1  3  0  1  0  0  0  1 91  2  1  1  0  0  0  1 %
--+ 
--+  Source  186 lines,  6032 lines per minute
--+  Lib    1982 bytes,  1749foam 48fsyme 6name 2file 2lazy 4type 2inl 2twins 2ext 2doc 16id
--+  Store  1368 K pool
--+ #1 (Fatal Error) Could not open file `bug-capital.ao'.
--+ #1 (Warning) Removing file `bug-CAPITAL.ao'.
#include "axllib.as"

print . "Hello world~n"
