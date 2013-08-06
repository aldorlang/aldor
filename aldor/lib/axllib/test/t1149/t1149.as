--> testint
--> testrun -l axllib
--> testrun -q3 -l axllib

--* From Manuel.Bronstein@sophia.inria.fr  Thu Sep  3 15:14:41 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA29524; Thu, 3 Sep 98 15:14:41 +0100
--* Received: from nirvana.inria.fr (bmanuel@nirvana.inria.fr [138.96.48.30])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id PAA09779 for <ax-bugs@nag.co.uk>; Thu, 3 Sep 1998 15:17:06 +0100 (BST)
--* Received: by nirvana.inria.fr (8.8.8/8.8.5) id QAA10047 for ax-bugs@nag.co.uk; Thu, 3 Sep 1998 16:13:18 +0200
--* Date: Thu, 3 Sep 1998 16:13:18 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199809031413.QAA10047@nirvana.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [4] -q2 yields wrong result at runtime

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 -fx optbug.as
-- Version: 1.1.11e
-- Original bug file name: optbug.as

------------------------------- optbug.as -----------------------------
--
-- Compiling at -q2 or above yields a wrong result (0 instead of 7):
--
-- % axiomxl -q1 -fx optbug.as
-- % optbug
-- dv(23,3) = 7
-- % axiomxl -q2 -fx optbug.as
-- % optbug
-- dv(23,3) = 0
-- 

#include "axllib"

macro {
	I == SingleInteger;
	F == SingleFloat;
}

local dv(t:I, n:I):F == {
	(nq, nr) := divide(n, 10);
	nr > 0 => (t quo n)::F;
	(t::F) / (n::F);
}

import from I, F;
print << "dv(23,3) = " << dv(23,3) << newline;
print << "dv(23,10) = " << dv(23,10) << newline;
print << (42@SingleInteger)::SingleFloat << newline;

