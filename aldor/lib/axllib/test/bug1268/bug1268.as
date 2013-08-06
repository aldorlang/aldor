--* From youssef@d0mino.fnal.gov  Fri Oct 20 09:27:13 2000
--* Received: from server-10.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id JAA15237
--* 	for <ax-bugs@nag.co.uk>; Fri, 20 Oct 2000 09:21:02 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 10497 invoked from network); 20 Oct 2000 08:20:32 -0000
--* Received: from d0mino.fnal.gov (131.225.224.45)
--*   by server-10.tower-4.starlabs.net with SMTP; 20 Oct 2000 08:20:32 -0000
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id DAA39223;
--* 	Fri, 20 Oct 2000 03:20:21 -0500 (CDT)
--* Date: Fri, 20 Oct 2000 03:20:21 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200010200820.DAA39223@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: bug0.as

--+ --
--+ -- axiomxl -g interp to get a core dump
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ Object2:with
--+     object:    (Obj:Type,val:Obj) -> %
--+     value: % ->(Obj:Type,val:Obj)
--+ == add
--+     Rep ==> Record(ob:Type,va:ob); import from Rep
--+     
--+     object     (Obj:Type,val:Obj):% == per [Obj,val]
--+     value(x:%):(Obj:Type,val:Obj)   == explode rep x
--+     
--+ FooDom(x:Object2):with
--+     ObjA:Type
--+     valA:ObjA
--+ == add
--+     (ObjA:Type,valA:ObjA) == value x 
--
-- axiomxl -g interp to get a core dump
--
#include "axllib"
#pile

Object2:with
    object:    (Obj:Type,val:Obj) -> %
    value: % ->(Obj:Type,val:Obj)
== add
    Rep ==> Record(ob:Type,va:ob); import from Rep
    
    object     (Obj:Type,val:Obj):% == per [Obj,val]
    value(x:%):(Obj:Type,val:Obj)   == explode rep x
    
FooDom(x:Object2):with
    ObjA:Type
    valA:ObjA
== add
    (ObjA:Type,valA:ObjA) == value x 

