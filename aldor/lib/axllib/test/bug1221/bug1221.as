--* From hemmecke@risc.uni-linz.ac.at  Tue May 30 17:53:52 2000
--* Received: from kernel.risc.uni-linz.ac.at (root@kernel.risc.uni-linz.ac.at [193.170.37.225])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id RAA00331
--* 	for <ax-bugs@nag.co.uk>; Tue, 30 May 2000 17:53:30 +0100 (BST)
--* Received: from deneb.risc.uni-linz.ac.at (deneb.risc.uni-linz.ac.at [193.170.37.113])
--* 	by kernel.risc.uni-linz.ac.at (8.9.2/8.9.2/Debian/GNU) with ESMTP id SAA18038;
--* 	Tue, 30 May 2000 18:52:44 +0200 (CEST)
--* Message-ID: <XFMail.000530185244.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.3 [p0] on Solaris
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* Date: Tue, 30 May 2000 18:52:44 +0200 (MET DST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* From: Ralf.Hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [1] problem with PrimitiveArray %

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -grun -laxllib -DC1 xxx.as
-- Version: Aldor version 1.1.12p5 for LINUX(glibc)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 30-MAY-2000
-- Aldor version 1.1.12p5 for LINUX(glibc) 
-- Subject: problem with PrimitiveArray %

-- The following code does compile with
--   axiomxl -V -grun -laxllib -DC1 xxx.as
-- with 1.1.12p2 and 1.1.12.5. For the latter, however, the execution
-- of the code will produce a runtime error which will not happen for
-- 1.1.12p2.

--: (Looking in CalixPowerProducts for 1 with code 169143988
--: Unhandled Exception: RuntimeError(??)
--: Export not found

-- Adding optimization option -q1 ... -q3 gives the same result.

#include "axllib"

macro {
        I == SingleInteger;
#if C1
        PAT == PrimitiveArray %;
#else
        PAT == List %;
#endif
}

define CAT: Category == with {
        <<: (TextWriter, %) -> TextWriter;
        1: %;
        one?: % -> Boolean;
    default {
        (p:TextWriter) << (x:%):TextWriter == p << "EINS";
        (x: %) = (y: %): Boolean == true;
        one?(x: %): Boolean == x = 1;
    }
}

CalixPowerProducts: CAT == add {
        Rep ==> I;
        import from Rep;

        thomas!(T: PAT): () == T.1;
        1: % == per 1;
        one?(x: %): Boolean == x = 1;
}

main(): () == {
        import from List String;
        import from CalixPowerProducts;
        e := 1;
        print << "(" << e << ")" << newline;
}
main();




