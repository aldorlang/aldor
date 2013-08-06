--* From hemmecke@risc.uni-linz.ac.at  Thu Nov  4 23:48:56 1999
--* Received: from kernel.risc.uni-linz.ac.at (root@kernel.risc.uni-linz.ac.at [193.170.36.225])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id XAA10899
--* 	for <ax-bugs@nag.co.uk>; Thu, 4 Nov 1999 23:48:47 GMT
--* Received: from gintonic.risc.uni-linz.ac.at (root@gintonic.risc.uni-linz.ac.at [193.170.33.250])
--* 	by kernel.risc.uni-linz.ac.at (8.9.2/8.9.2/Debian/GNU) with ESMTP id AAA30758
--* 	for <ax-bugs@nag.co.uk>; Fri, 5 Nov 1999 00:48:37 +0100 (CET)
--* Received: by risc.uni-linz.ac.at
--* 	via send-mail from stdin
--* 	id <m11jWcP-000YhOC@gintonic.risc.uni-linz.ac.at> (Debian Smail3.2.0.102)
--* 	for ax-bugs@nag.co.uk; Fri, 5 Nov 1999 00:48:37 +0100 (CET) 
--* Message-Id: <m11jWcP-000YhOC@gintonic.risc.uni-linz.ac.at>
--* Date: Fri, 5 Nov 1999 00:48:37 +0100 (CET)
--* From: hemmecke@risc.uni-linz.ac.at (Ralf HEMMECKE)
--* To: ax-bugs@nag.co.uk
--* Subject: [9] with vs. with

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DC1 xxx.as
-- Version: Aldor version 1.1.12p2 for SPARC [Solaris: GCC]
-- Original bug file name: xxx.as

-----------------------------------------------------------------------
-- 05-NOV-99
-----------------------------------------------------------------------
-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 05-NOV-99
-- Aldor version 1.1.12p2 for SPARC [Solaris: GCC] 
-- Subject: with vs. with


-- Calling sequence:
-- Problem case:
--   axiomxl -DC1 xxx.as
--:"xxxx.as", line 22:         variable(s: String):% == variable index s;
--:                    ..........................................^
--:[L22 C43] #1 (Error) There are no suitable meanings for the operator `index'.

-- Of course there is no error in the #else part, the question is, whether
-- the C1 case is correct syntax for defining a category in case one
-- wants to have all functions from CAT1 also available in CAT2.

#include "axllib"

macro {
	I == SingleInteger;
}

define CAT1: Category == Join(Monoid,Order) with {
	index: String -> I;
	variable: I -> %;
}

define CAT2: Category == 

#if C1
   with { CAT1;
#else
   CAT1 with {
#endif

	variable: String -> %;
    default {
	variable(s: String):% == variable index s;
    }
}
