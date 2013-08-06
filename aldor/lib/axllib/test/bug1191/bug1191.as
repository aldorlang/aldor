--* From Manuel.Bronstein@sophia.inria.fr  Fri Feb 11 11:32:08 2000
--* Received: from kistren.inria.fr (IDENT:root@kistren.inria.fr [138.96.111.5])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id LAA12096
--* 	for <ax-bugs@nag.co.uk>; Fri, 11 Feb 2000 11:32:05 GMT
--* Received: by kistren.inria.fr (8.8.8/8.8.5) id MAA00797 for ax-bugs@nag.co.uk; Fri, 11 Feb 2000 12:32:37 +0100
--* Date: Fri, 11 Feb 2000 12:32:37 +0100
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200002111132.MAA00797@kistren.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [6] bit$Machine differs on SInt or BInt

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.12p4
-- Original bug file name: badbit.as

-------------------------------- badbit.as ---------------------------------
--
-- bit$Machine does not have the same semantics for SingleInteger and Integer
--

#include "axllib"

import from SingleInteger, Integer;

a:SingleInteger := -5;
print << bit(a, 5) << newline;                  -- true
print << bit(a::Integer, 5) << newline;         -- false

