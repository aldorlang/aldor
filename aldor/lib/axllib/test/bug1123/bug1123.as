--* From hemmecke@aix550.informatik.uni-leipzig.de  Tue Jul  8 14:17:04 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA20236; Tue, 8 Jul 97 14:17:04 +0100
--* Received: from server1.rz.uni-leipzig.de (root@server1.rz.uni-leipzig.de [139.18.1.1])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id OAA11329 for <ax-bugs@nag.co.uk>; Tue, 8 Jul 1997 14:17:54 +0100 (BST)
--* Received: from aix550.informatik.uni-leipzig.de (aix550.informatik.uni-leipzig.de [139.18.2.14])
--* 	by server1.rz.uni-leipzig.de (8.8.5/8.8.5) with SMTP id PAA29729
--* 	for <ax-bugs@nag.co.uk>; Tue, 8 Jul 1997 15:15:44 +0200 (MESZ)
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA18278; Tue, 8 Jul 1997 15:16:58 +0100
--* Date: Tue, 8 Jul 1997 15:16:58 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9707081416.AA18278@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@nag.co.uk
--* Subject: [8] default functions inheritance

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DC1 xxx.as
-- Version: AXIOM-XL version 1.1.9a for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 08-JUL-97
-- AXIOM-XL version 1.1.9a for AIX RS/6000
-- Subject: default functions inheritance

-- Compiling this little piece of code with
--
--   axiomxl -DC1 xxx.as
--
-- gives the error:

--:"xxx.as", line 20:     g():Boolean == f();
--:                   ...................^
--:[L20 C20] #1 (Error) There are no suitable meanings for the operator `f'.

-- whereas 
--   axiomxl xxx.as
-- works fine.

-- Before I detected this behaviour (with a larger piece of code), the 
-- compiler suggested to 
--   import <something> from %
-- After adding an 
--   import from %
-- (use assertion C2) I was given the 

--:  (Warning) Ignoring explicit import from %.

-- which puzzled me a lot.

-- I also tried qualifying (assertion C3).
--   axiomxl -DC1 -DC3 xxx.as
-- does compile on my machine, but I was not sure whether it will really work
-- as expected, because it looks somewhat strange to me.

#include "axllib"
define Cat1:Category == with {
  f: () -> Boolean;
  default {
    f():Boolean == true;
  }
}	

define Cat2:Category == 
#if C1
  with {
  Cat1;
#else
  Cat1 with {
#endif
  g: () -> Boolean;
  default {
#if C2
    import from %;
#endif
    g():Boolean == f()
#if C3
$%
#endif
;
  }
}
