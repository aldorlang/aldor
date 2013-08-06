--* From youssef@d0mino.fnal.gov  Thu Oct 12 07:43:27 2000
--* Received: from d0mino.fnal.gov (d0mino.fnal.gov [131.225.224.45])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id HAA12645
--* 	for <ax-bugs@nag.co.uk>; Thu, 12 Oct 2000 07:43:25 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id BAA07722;
--* 	Thu, 12 Oct 2000 01:43:26 -0500 (CDT)
--* Date: Thu, 12 Oct 2000 01:43:26 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200010120643.BAA07722@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: currybug.as

--+ --
--+ --  Hi Martin,
--+ --     Here's a very minor problem.  The compiler doesn't seem to like it
--+ --  when a line is split in the middle of a curried function.
--+ --
--+ --   Saul
--+ --
--+ #include "axllib"
--+ 
--+ S ==> SingleInteger;
--+ 
--+ f(a:S)(b:S)(c:S):S == a + b + c;
--+ g(a:S)(b:S)_                     
--+      (c:S):S == a + b + c;
--+ 
--
--  Hi Martin,
--     Here's a very minor problem.  The compiler doesn't seem to like it
--  when a line is split in the middle of a curried function.
--
--   Saul
--
#include "axllib"

S ==> SingleInteger;

f(a:S)(b:S)(c:S):S == a + b + c;
g(a:S)(b:S)_                     
     (c:S):S == a + b + c;

