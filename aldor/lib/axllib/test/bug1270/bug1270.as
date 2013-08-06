--* From youssef@d0mino.fnal.gov  Sun Oct 22 03:17:32 2000
--* Received: from server-20.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id DAA29157
--* 	for <ax-bugs@nag.co.uk>; Sun, 22 Oct 2000 03:17:31 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 29550 invoked from network); 22 Oct 2000 02:17:00 -0000
--* Received: from d0mino.fnal.gov (131.225.224.45)
--*   by server-20.tower-4.starlabs.net with SMTP; 22 Oct 2000 02:17:00 -0000
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id VAA63308;
--* 	Sat, 21 Oct 2000 21:17:00 -0500 (CDT)
--* Date: Sat, 21 Oct 2000 21:17:00 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200010220217.VAA63308@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: bug.as

--+ --
--+ --  Martin,  
--+ --     Do axiomxl -g interp bug.as to see a mysterious core dump.
--+ -- 
--+ --    Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 		
--+ define MathCategory(Obj:Type,Mor:(A:Obj,B:Obj)->with):Category == with
--+     compose: (A:Obj,B:Obj,C:Obj) -> (g:Mor(B,C),f:Mor(A,B)) -> Mor(A,C)
--+     id: (A:Obj) -> Mor(A,A)
--+ 
--+ ProductObj(ObjA:Category,ObjB:Category): with
--+     product:     (ObjA,ObjB) -> %
--+     value:  % -> (ObjA,ObjB)
--+ == add
--+     Rep == Record(a:ObjA,b:ObjB); import from Rep
--+     product(A:ObjA,B:ObjB):% == per [A,B]
--+     value(po:%):(ObjA,ObjB) == explode rep po
--+     
--+ ProductMor(ObjA:Category,MorA:(X:ObjA,Y:ObjA)->with,ObjB:Category,MorB:(X:ObjB,Y:ObjB)->with)(X:ProductObj(ObjA,ObjB),Y:ProductObj(ObjA,ObjB)):with
--+     Xa: ObjA
--+     Xb: ObjB
--+     Ya: ObjA
--+     Yb: ObjB
--+     product:      (MorA(Xa,Ya),MorB(Xb,Yb)) -> %
--+     value:   % -> (MorA(Xa,Ya),MorB(Xb,Yb))
--+ == add
--+     (Xa:ObjA,Xb:ObjB) == value X
--+     (Ya:ObjA,Yb:ObjB) == value Y
--+     Rep == Record(ma:MorA(Xa,Ya),mb:MorB(Xb,Yb)); import from Rep
--+     product  (fa:MorA(Xa,Ya),fb:MorB(Xb,Yb)):% == per [fa,fb]
--+     value(pm:%):(MorA(Xa,Ya),   MorB(Xb,Yb)) == error " " -- explode rep pm
--+ 
--
--  Martin,  
--     Do axiomxl -g interp bug.as to see a mysterious core dump.
-- 
--    Saul
--
#include "axllib"
#pile
		
define MathCategory(Obj:Type,Mor:(A:Obj,B:Obj)->with):Category == with
    compose: (A:Obj,B:Obj,C:Obj) -> (g:Mor(B,C),f:Mor(A,B)) -> Mor(A,C)
    id: (A:Obj) -> Mor(A,A)

ProductObj(ObjA:Category,ObjB:Category): with
    product:     (ObjA,ObjB) -> %
    value:  % -> (ObjA,ObjB)
== add
    Rep == Record(a:ObjA,b:ObjB); import from Rep
    product(A:ObjA,B:ObjB):% == per [A,B]
    value(po:%):(ObjA,ObjB) == explode rep po
    
ProductMor(ObjA:Category,MorA:(X:ObjA,Y:ObjA)->with,ObjB:Category,MorB:(X:ObjB,Y:ObjB)->with)(X:ProductObj(ObjA,ObjB),Y:ProductObj(ObjA,ObjB)):with
    Xa: ObjA
    Xb: ObjB
    Ya: ObjA
    Yb: ObjB
    product:      (MorA(Xa,Ya),MorB(Xb,Yb)) -> %
    value:   % -> (MorA(Xa,Ya),MorB(Xb,Yb))
== add
    (Xa:ObjA,Xb:ObjB) == value X
    (Ya:ObjA,Yb:ObjB) == value Y
    Rep == Record(ma:MorA(Xa,Ya),mb:MorB(Xb,Yb)); import from Rep
    product  (fa:MorA(Xa,Ya),fb:MorB(Xb,Yb)):% == per [fa,fb]
    value(pm:%):(MorA(Xa,Ya),   MorB(Xb,Yb)) == error " " -- explode rep pm


