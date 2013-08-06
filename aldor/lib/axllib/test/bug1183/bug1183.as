--* From hemmecke@risc.uni-linz.ac.at  Tue Nov 23 18:50:17 1999
--* Received: from kernel.risc.uni-linz.ac.at (root@[193.170.36.225])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id SAA17900
--* 	for <ax-bugs@nag.co.uk>; Tue, 23 Nov 1999 18:49:58 GMT
--* Received: from iapetus.risc.uni-linz.ac.at (root@iapetus.risc.uni-linz.ac.at [193.170.36.25])
--* 	by kernel.risc.uni-linz.ac.at (8.9.2/8.9.2/Debian/GNU) with ESMTP id TAA14336
--* 	for <ax-bugs@nag.co.uk>; Tue, 23 Nov 1999 19:48:59 +0100 (CET)
--* Received: by risc.uni-linz.ac.at
--* 	via send-mail from stdin
--* 	id <m11qKzr-0025TNC@iapetus.risc.uni-linz.ac.at> (Debian Smail3.2.0.102)
--* 	for ax-bugs@nag.co.uk; Tue, 23 Nov 1999 19:48:59 +0100 (CET) 
--* Message-Id: <m11qKzr-0025TNC@iapetus.risc.uni-linz.ac.at>
--* Date: Tue, 23 Nov 1999 19:48:59 +0100 (CET)
--* From: hemmecke@risc.uni-linz.ac.at (Ralf HEMMECKE)
--* To: ax-bugs@nag.co.uk
--* Subject: [1] function of dependent types

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DC1 xxx.as
-- Version: Aldor version 1.1.12p2 for SPARC [Solaris: GCC]
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 23-NOV-99
-- Aldor version 1.1.12p2 for SPARC [Solaris: GCC] 
-- Subject: function of dependent types

-- The code does not compile for
--   axiomxl -DC1 xxx.as

--:"xxx.as", line 3: foo(X: Type,
--:                  ....^
--:[L3 C5] #1 (Error) Have determined 1 possible types for the expression.
--:	Meaning 1: (X: Type, Y:  with y: X -> %, Z:  with z: Y -> %) -> ()
--:  The context requires an expression of type (X: Type, Y:  with y: X -> %, Z:  with z: Y -> %) -> ().

-- but works fine for
--   axiomxl -DC2 xxx.as

#include "axllib"

foo(X: Type,
    Y: with {y: X -> %}
#if C1
    , Z: with {z: Y -> %}
#endif
): () == return;
