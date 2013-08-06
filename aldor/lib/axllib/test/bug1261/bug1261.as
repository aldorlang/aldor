--* From youssef@d0mino.fnal.gov  Thu Oct 12 07:41:15 2000
--* Received: from d0mino.fnal.gov (d0mino.fnal.gov [131.225.224.45])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id HAA12506
--* 	for <ax-bugs@nag.co.uk>; Thu, 12 Oct 2000 07:41:14 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id BAA06145;
--* 	Thu, 12 Oct 2000 01:41:13 -0500 (CDT)
--* Date: Thu, 12 Oct 2000 01:41:13 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200010120641.BAA06145@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: objectbug.as

--+ --
--+ --  Hi Martin,
--+ --     Here's perhaps the simplest dependent type bug that I've found.
--+ --  If you compile this (e.g. with axiomxl -g interp) it will core dump.
--+ --  It would be very nice for me if this could be made to work or if you
--+ --  can think of some workaround.  In general, these types of "Object" like
--+ --  types are pretty essential.  
--+ -- 
--+ --     Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ MC ==> Record(Obj:Type,Mor:(Obj,Obj)->with)
--+ 
--+ define FunctorCategory(A:MC,B:MC):Category == with
--+     Obj:Type
--+     Mor:(Obj,Obj)->with
--+     
--+ Functor(A:MC,B:MC):FunctorCategory(A,B) with == add
--+     (Obj:Type,Mor:(Obj,Obj)->with) == explode A
--
--  Hi Martin,
--     Here's perhaps the simplest dependent type bug that I've found.
--  If you compile this (e.g. with axiomxl -g interp) it will core dump.
--  It would be very nice for me if this could be made to work or if you
--  can think of some workaround.  In general, these types of "Object" like
--  types are pretty essential.  
-- 
--     Saul
--
#include "axllib"
#pile

MC ==> Record(Obj:Type,Mor:(Obj,Obj)->with)

define FunctorCategory(A:MC,B:MC):Category == with
    Obj:Type
    Mor:(Obj,Obj)->with
    
Functor(A:MC,B:MC):FunctorCategory(A,B) with == add
    (Obj:Type,Mor:(Obj,Obj)->with) == explode A
