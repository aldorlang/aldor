--* From hemmecke@aix550.informatik.uni-leipzig.de  Mon Apr  7 13:34:59 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA09806; Mon, 7 Apr 97 13:34:59 +0100
--* Received: from server1.rz.uni-leipzig.de (daemon@server1.rz.uni-leipzig.de [139.18.1.1])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id NAA18388 for <ax-bugs@nag.co.uk>; Mon, 7 Apr 1997 13:36:04 +0100 (BST)
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.20/16.2) id AA179706507; Mon, 7 Apr 1997 14:35:07 +0200
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA18785; Mon, 7 Apr 1997 14:39:16 +0100
--* Date: Mon, 7 Apr 1997 14:39:16 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9704071339.AA18785@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] default and package(%)

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fx xxx.as
-- Version: AXIOM-XL version 1.1.8c for AIX RS/6000
-- Original bug file name: xxx.as

-----------------------------------------------------------------------
-- 07-APR-97
-----------------------------------------------------------------------
-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 07-APR-96
-- AXIOM-XL version 1.1.8c for AIX RS/6000 
-- Subject: default and package(%)

-- Compile the following code with
--   axiomxl -fx xxx.as 
-- then start the program.
-- Error message:

--:Segmentation fault(coredump)

-----------------------------------------------------
-- xxx.as
----------------------------------------------------
#include "axllib"

macro {
  B == Boolean;
  MC == BasicType with {1:%;*:(%, %) -> %;}
}

define MONOID:Category == Monoid with {
  default {
    (x:%) ^ (n:Integer):% == power(1,x,n)$BinaryPowering(%,*,Integer);
  }
}

M(R:MC):MONOID with == add {
  Rep ==> R;
  import from Rep;
  inline from Rep;
  (p:TextWriter) << (x:%):TextWriter == p << rep x;
  1:% == per(1$Rep);
  sample:% == per(1$Rep);
  (x:%)*(y:%):% == per(rep x * rep y);
  (x:%)=(y:%):B == rep x = rep y;
}

Test():() == {
X == M Integer;
import from X, Integer;
(1$X)^1;
}
Test();

