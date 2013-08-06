--* From Manuel.Bronstein@sophia.inria.fr  Wed Jul 29 19:56:30 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA24840; Wed, 29 Jul 98 19:56:30 +0100
--* Received: from nirvana.inria.fr (bmanuel@nirvana.inria.fr [138.96.48.30])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id TAA08291 for <ax-bugs@nag.co.uk>; Wed, 29 Jul 1998 19:59:45 +0100 (BST)
--* Received: by nirvana.inria.fr (8.8.8/8.8.5) id UAA12744 for ax-bugs@nag.co.uk; Wed, 29 Jul 1998 20:56:57 +0200
--* Date: Wed, 29 Jul 1998 20:56:57 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199807291856.UAA12744@nirvana.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] optimizer mangles constant at -q2

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 -fx optbug.as
-- Version: 1.1.11e
-- Original bug file name: optbug.as

------------------------------- optbug.as ------------------------
--
-- This is a bug in the optimizer with large (40-bits) constants

#include "axllib"

import from SingleInteger;

-- this constant is 2^40, no problem on a 64-bit machine
print << "q = " << 1099511627776 << newline;

-- On a DEC-alpha I get the following:
-- % axiomxl -q1 -fx optbug.as
-- % optbug
-- q = 1099511627776
-- 
-- % axiomxl -q2 -fx optbug.as
-- % optbug
-- q = 0
-- 
-- Looking at the generated C-code, I see for -q1:
--   T5 = fiCCall1(FiWord, l0->X10_integer, (FiWord) "1099511627776");
-- and for -q2, the print statement got inlined into:
--      fiCCall3(FiWord, T5, (FiWord) "q = ", (FiWord) 1, (FiWord) 0);
--      T11 = formatSInt(0);
-- SO THE OPTIMIZER "ATE" THE HIGH 32-BIT PART OF MY CONSTANT!

