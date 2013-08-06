--* From youssef@d0lxbld4.fnal.gov  Mon Jun 12 09:50:25 2000
--* Received: from d0lxbld4.fnal.gov (d0lxbld4.fnal.gov [131.225.225.19])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id JAA07726
--* 	for <ax-bugs@nag.co.uk>; Mon, 12 Jun 2000 09:50:24 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0lxbld4.fnal.gov (8.9.3/8.9.3) id DAA03095
--* 	for ax-bugs@nag.co.uk; Mon, 12 Jun 2000 03:50:03 -0500
--* Date: Mon, 12 Jun 2000 03:50:03 -0500
--* From: Saul Youssef <youssef@d0lxbld4.fnal.gov>
--* Message-Id: <200006120850.DAA03095@d0lxbld4.fnal.gov>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] -g Run and -Fx seem to fail on "hello world"

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx file.as
-- Version: 1.1.12
-- Original bug file name: bug.as

--+ --
--+ --  In Aldor version 1.1.12 for LINUX, this program works fine with
--+ --
--+ --     axiomxl -g interp bug.as
--+ --
--+ --  but
--+ --
--+ --     axiomxl -g run bug.as 
--+ --
--+ --  produces no output and
--+ --
--+ --     axiomxl -Fx bug.as
--+ --     bug
--+ --
--+ --  core dumps.  
--+ --
--+ --      Saul Youssef, youssef@fnal.gov
--+ --
--+ #include "axllib" 
--+ 
--+ print << "hello" << newline;
--+ 
--+ 
--+ 
--+ 
--+ 
--
--  In Aldor version 1.1.12 for LINUX, this program works fine with
--
--     axiomxl -g interp bug.as
--
--  but
--
--     axiomxl -g run bug.as 
--
--  produces no output and
--
--     axiomxl -Fx bug.as
--     bug
--
--  core dumps.  
--
--      Saul Youssef, youssef@fnal.gov
--
#include "axllib" 

print << "hello" << newline;





