--* From Manuel.Bronstein@sophia.inria.fr  Wed Oct 27 14:50:08 1999
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id OAA00651
--* 	for <ax-bugs@nag.co.uk>; Wed, 27 Oct 1999 14:49:42 +0100 (BST)
--* Received: by droopix.inria.fr (8.8.8/8.8.5) id PAA19955 for ax-bugs@nag.co.uk; Wed, 27 Oct 1999 15:49:11 +0200
--* Date: Wed, 27 Oct 1999 15:49:11 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199910271349.PAA19955@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] compiler removes ao file

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl foo.as; axiomxl -ginterp foo.ap
-- Version: 1.1.12p2
-- Original bug file name: foo.as

----------------------------- foo.as -------------------------
--
-- axiomxl -ginterp foo.xx
--   removes foo.ao if foo.xx does not exist.
--   this means than an .ao file can be destroyed because of
--   a typo in the extension (p instead of o)
-- The compiler should not remove files following a 'Fatal Error'
--
-- % axiomxl foo.as                  % foo.ao  created ok
-- % axiomxl -ginterp foo.ap
-- #1 (Fatal Error) Could not open file `foo.ap'.
-- #1 (Warning) Removing file `foo.ao'.
--

#include "axllib"

print << "hello" << newline;
