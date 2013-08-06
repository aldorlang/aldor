--* From youssef@d0mino.fnal.gov  Sat Aug 26 20:37:43 2000
--* Received: from d0mino.fnal.gov (d0mino.fnal.gov [131.225.224.45])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id UAA02422
--* 	for <ax-bugs@nag.co.uk>; Sat, 26 Aug 2000 20:37:42 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id OAA87881;
--* 	Sat, 26 Aug 2000 14:37:31 -0500 (CDT)
--* Date: Sat, 26 Aug 2000 14:37:31 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200008261937.OAA87881@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: bug4.as

--+ --
--+ --  Hi,
--+ --      It seems to me that this demonstrates a bug.  If I understand this
--+ --  correctly, Function(A)(X) has type Category and value 
--+ --  "with { *:(%,%) -> % }".  Both domains below compile with axiomxl -g interp,
--+ --  but Dom does not supply the needed signature.
--+ --
--+ --    Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ Function(A:Category):(A->Category) == (a:A):Category +-> with { *:(%,%)->% } 
--+     
--+ Dom(A:Category,X:A):Function(A)(X) == add
--+ 
--+ Dom2(A:Category,X:A):Function(A)(X) == add { (x:%)+(y:%):% == error " " }
--+ 
--
--  Hi,
--      It seems to me that this demonstrates a bug.  If I understand this
--  correctly, Function(A)(X) has type Category and value 
--  "with { *:(%,%) -> % }".  Both domains below compile with axiomxl -g interp,
--  but Dom does not supply the needed signature.
--
--    Saul
--
#include "axllib"
#pile

Function(A:Category):(A->Category) == (a:A):Category +-> with { *:(%,%)->% } 
    
Dom(A:Category,X:A):Function(A)(X) == add

Dom2(A:Category,X:A):Function(A)(X) == add { (x:%)+(y:%):% == error " " }

