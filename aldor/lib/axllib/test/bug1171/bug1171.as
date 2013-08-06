--* From Manuel.Bronstein@sophia.inria.fr  Fri Oct  8 11:44:50 1999
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id LAA06792
--* 	for <ax-bugs@nag.co.uk>; Fri, 8 Oct 1999 11:44:31 +0100 (BST)
--* Received: by droopix.inria.fr (8.8.8/8.8.5) id MAA12848 for ax-bugs@nag.co.uk; Fri, 8 Oct 1999 12:40:03 +0200
--* Date: Fri, 8 Oct 1999 12:40:03 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199910081040.MAA12848@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [7] undefined symbol in libfoam-gmp.a

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fx -cruntime=foam-gmp,gmp hello.as
-- Version: 1.1.12p2 for SPARC [gcc]
-- Original bug file name: hello.as

------------------------------- hello.as -------------------------------
--
-- libfoam-gmp.a for Solaris in the 1.1.12p has an unreferenced symbol!
--
-- % axiomxl -fx hello.as
--
-- % axiomxl -fx -cruntime=foam-gmp,gmp hello.as
-- Undefined      first referenced
--  symbol           in file
-- mcount            /net/safir/lib/axiomxl/sun4OS5/lib/libfoam-gmp.a(runtime.o)
-- ld: fatal: Symbol referencing errors. No output written to hello
-- collect2: ld returned 1 exit status
-- #1 (Fatal Error) Linker failed.  Command was:
-- unicl hello.o axlmain.o -L. -L/net/safir/lib/axiomxl/sun4OS5/share/lib
-- -L/net/safir/lib/axiomxl/sun4OS5/lib -o  hello -laxllib -lfoam-gmp -lgmp

#include "axllib"

print << "hello" << newline;
