--* From hemmecke@risc.uni-linz.ac.at  Thu May 11 13:17:37 2000
--* Received: from kernel.risc.uni-linz.ac.at (root@kernel.risc.uni-linz.ac.at [193.170.37.225])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id NAA18188
--* 	for <ax-bugs@nag.co.uk>; Thu, 11 May 2000 13:17:36 +0100 (BST)
--* Received: from deneb.risc.uni-linz.ac.at (deneb.risc.uni-linz.ac.at [193.170.37.113])
--* 	by kernel.risc.uni-linz.ac.at (8.9.2/8.9.2/Debian/GNU) with ESMTP id OAA22498;
--* 	Thu, 11 May 2000 14:17:29 +0200 (CEST)
--* Message-ID: <XFMail.000511141729.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.3 [p0] on Solaris
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* Date: Thu, 11 May 2000 14:17:29 +0200 (MET DST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* From: Ralf.Hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [2] Using a computed constant as a domain parameter

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib xxx.as
-- Version: Aldor version 1.1.12p5 for LINUX(glibc)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 11-MAY-2000
-- Aldor version 1.1.12p5 for LINUX(glibc) 
-- Subject: Using a computed constant as a domain parameter

-- The following piece of code might give different results for another
-- compilation.
-- Compile with 
--   axiomxl -laxllib -grun xxx.as
-- which gives the output

--: %-- Computation is done over Z.
--: 1=1
--: 2=2
--: 3=3
--: 4=4
--: %-- Working modulo 2
--: 1=1
--: 2=1073744158
--: 3=1073744159
--: 4=1073744160
--: %-- Working modulo 3
--: 1=1
--: 2=1073744158
--: 3=1073744159
--: 4=1073744160

-- on my machine.

-- For 
--   axiomxl -laxllib -grun -DC1 xxx.as
-- the output will correctly be
 
--: %-- Computation is done over Z.
--: 1=1
--: 2=2
--: 3=3
--: 4=4
--: %-- Working modulo 2
--: 1=1
--: 2=0
--: 3=1
--: 4=0
--: %-- Working modulo 3
--: 1=1
--: 2=2
--: 3=0
--: 4=1

#include "axllib"

CxGBC(
    R: with {
        <<: (TextWriter, %) -> TextWriter;
        +: (%, %) -> %;
        1: %;
    }
): with {
        sb: () -> ();
} == add {
        sb(): () == {
                r: R := 1;
                error << "1=" << r << newline;
                error << "2=" << r+r << newline;
                error << "3=" << r+r+r << newline;
                error << "4=" << r+r+r+r << newline;
        }
}

CxGBC: with {
        sb: List String -> ();
} == add {
        sb(ex: List String): () == {
                import from List String;
                import from NumberScanPackage SingleInteger;
                ring == first ex;
                if ring = "Q" then {
                        error << "%-- Computation is done over Z." << newline;
                        sb() $ CxGBC(Integer);
                } else {
                        p: SingleInteger == scanNumber(ring);
                        error << "%-- Working modulo " << p << newline;
#if C1
                        sb() $ CxGBC(SingleIntegerMod scanNumber ring);
#else
                        sb() $ CxGBC(SingleIntegerMod p);
#endif
                }
        }
}

main():() == {
        import from List String;
        sb(["Q"]) $ CxGBC;
        sb(["2"]) $ CxGBC;
        sb(["3"]) $ CxGBC;
}
main();
