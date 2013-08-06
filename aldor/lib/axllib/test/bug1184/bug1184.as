--* From Manuel.Bronstein@sophia.inria.fr  Mon Nov 29 09:38:33 1999
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id JAA09244
--* 	for <ax-bugs@nag.co.uk>; Mon, 29 Nov 1999 09:38:32 GMT
--* Received: by droopix.inria.fr (8.8.8/8.8.5) id KAA04552 for ax-bugs@nag.co.uk; Mon, 29 Nov 1999 10:38:10 +0100
--* Date: Mon, 29 Nov 1999 10:38:10 +0100
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199911290938.KAA04552@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [5] optimizer doesn't inline triviality

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q3 -fc optbug.as
-- Version: 1.1.12p2
-- Original bug file name: optbug.as

----------------------------- optbug.as --------------------------------
--
-- The function g() below is optimized correctly at -q3, but not
-- if it is inside an extend construct!
--

#include "axllib"

macro I == SingleInteger;

-- This one is optimized correctly:
--         return (FiWord) ((FiSInt) P0_a - (FiSInt) P1_b);
g(a:I, b:I):I == a - b;

-- But not this one:
--        l1 = (PFmt7) fiEnvLevel(e1);
--        return fiCCall2(FiWord, l1->X1__MINUS_, P0_a, P1_b);
extend I: with { f: (%, %) -> % } == add { f(a:%, b:%):% == a - b; }
