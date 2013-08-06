--* From hemmecke@aix550.informatik.uni-leipzig.de  Tue Apr 22 12:06:17 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA14224; Tue, 22 Apr 97 12:06:17 +0100
--* Received: from server1.rz.uni-leipzig.de (daemon@server1.rz.uni-leipzig.de [139.18.1.1])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id MAA16554 for <ax-bugs@nag.co.uk>; Tue, 22 Apr 1997 12:06:50 +0100 (BST)
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.20/16.2) id AA143667148; Tue, 22 Apr 1997 13:05:48 +0200
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA15967; Tue, 22 Apr 1997 13:10:26 +0100
--* Date: Tue, 22 Apr 1997 13:10:26 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9704221210.AA15967@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] constant defaults

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl xxx.as
-- Version: AXIOM-XL version 1.1.9a for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 22-APR-96
-- AXIOM-XL version 1.1.9a for AIX RS/6000 
-- Subject: constant default

-- Compile the following code with
--   axiomxl xxx.as 
-- then enter the following into axiomxl -gloop

--+ #include "axllib"
--+ #library XXX "xxx.ao"
--+ import from XXX;
--+ X == UT "x";
--+ import from SingleInteger,X;
--+ numberOfVariables;
--+ var 1;
--+ var 1;

-- Following messages appear:

--: %1 >> #include "axllib"
--:                                            Comp: 860 msec, Interp: 40 msec
--: %2 >> #library XXX "xxx.ao"
--: %3 >> import from XXX;
--:                                            Comp: 10 msec, Interp: 0 msec
--: %4 >> X == UT "x";
--: Defined X @ ? == UT("x")
--:                                            Comp: 10 msec, Interp: 170 msec
--: %5 >> import from SingleInteger,X;
--:                                            Comp: 260 msec, Interp: 0 msec
--: %6 >> numberOfVariables;
--: 1 @ SingleInteger
--:                                            Comp: 70 msec, Interp: 130 msec
--: %7 >> var 1;
--: Infinite number of variables.
--: #0  8128be0 in <error> at unit [basic]
--: #1     c6ce in <lazyGetter> at unit [runtime]
--: #2      cca in <PPC> at unit [xxx]
--: #3     6f0f in <get1> at unit [runtime]
--: #4     6c76 in <get0> at unit [runtime]
--: #5     3a45 in <get> at unit [runtime]
--: #6     8adf in <domainGetExport!> at unit [runtime]
--: #7     ab1b in <rtDelayedGetExport!> at unit [runtime]
--: #8     c6b1 in <lazyGetter> at unit [runtime]
--: #9     124f in <<<> at unit [xxx]
--: #10     c39e in <lazyGetter> at unit [runtime]
--: #11      13f in <-> at unit [main]
--: #12        0 in <(null)> at unit [`dispose! s' indicates that `s' will no longer be used.
--: ]
--: ...
--:  @ X
--:                                            Comp: 20 msec, Interp: 20 msec
--: %8 >> var 1;
--: x @ X


-- If one uses the assertion CA or CB, or if one replaces the
-- signature of 
--   numberOfVariables
-- to
--   () -> I
-- no error will occur.


-----------------------------------------------------
-- xxx.as
----------------------------------------------------
#include "axllib"

macro {
  B == Boolean;
  I == SingleInteger;
}

define PPC: Category ==
  BasicType with {
  numberOfVariables: I;
  var: I -> %;
  one?: % -> B;
  default {
#if CA
#else
    numberOfVariables:I == error "Infinite number of variables.";
#endif
    one?(x:%):B == true;
  }
}

UT(var:String): PPC == add {
  Rep ==> I; import from Rep;
  (p:TextWriter) << (x:%):TextWriter == {
    if one? x then p << var else p << var << "^" << rep x;
  }
#if CB
  one?(x:%):B == false;  
#endif
  (x:%)=(y:%):B == true;
  numberOfVariables: I == 1;
  sample:% == per 0;
  var(i:I):% == per 1;
}
