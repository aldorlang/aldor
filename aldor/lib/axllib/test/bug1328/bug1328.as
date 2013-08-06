--* From hemmecke@risc.uni-linz.ac.at  Wed Sep 19 12:19:29 2001
--* Received: from mail-delivery-1.star.net.uk (welly-1.star.net.uk [195.216.16.165])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id MAA15476
--* 	for <ax-bugs@nag.co.uk>; Wed, 19 Sep 2001 12:19:28 +0100 (BST)
--* From: hemmecke@risc.uni-linz.ac.at
--* Received: (qmail 32301 invoked from network); 19 Sep 2001 11:18:59 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by delivery-1.star-private-mail-4.star.net.uk with SMTP; 19 Sep 2001 11:18:59 -0000
--* Received: (qmail 21665 invoked from network); 19 Sep 2001 11:18:58 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 19 Sep 2001 11:18:58 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 31594 invoked from network); 19 Sep 2001 11:02:25 -0000
--* Received: from kernel.risc.uni-linz.ac.at (193.170.37.225)
--*   by server-14.tower-17.messagelabs.com with SMTP; 19 Sep 2001 11:02:25 -0000
--* Received: from enceladus.risc.uni-linz.ac.at (hemmecke@thea.risc.uni-linz.ac.at [193.170.38.91])
--* 	by kernel.risc.uni-linz.ac.at (8.11.6/8.11.6) with ESMTP id f8JB45010829;
--* 	Wed, 19 Sep 2001 13:04:05 +0200
--* Message-ID: <XFMail.20010919130402.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.4.4 on Linux
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* Date: Wed, 19 Sep 2001 13:04:02 +0200 (CEST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [7]category default and qualification

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -V -Q0 -Fx -laxllib -DCA xxx.as
-- Version: Aldor version 1.0.-1(3) for LINUX(glibc2.1) 
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 19-SEP-2001
-- Aldor version 1.0.-1(3) for LINUX(glibc2.1) 
-- Subject: category default and qualification

-- The original intention of this piece of code was to invoke a
-- function init() at the time the domain is instantiated in order to
-- initialise an array with some values depending on the domain
-- parameters.

-- Compilation 
--   aldor -V -Q0 -Fx -laxllib xxx.as
-- results in a program that works.
-- With option -DCA, however,
--   aldor -V -Q0 -Fx -laxllib -DCA xxx.as
-- the executable crashes after printing the following:
--: INIT:0
--: VAR: 1
--: x%: :x^1*y^0
--: Segmentation fault

-- The program started by 
--   aldor -V -Q0 -grun -laxllib -DCA xxx.as
-- will not crash. However, it only prints the "y:" line and not the
-- "z:" line.

#include "axllib"

macro {
        N == SingleInteger; 
        nl == newline;
        TW == TextWriter;
}

CalixPowerProducts: with {
        <<: (TW, %) -> TW;
        names: () -> List String;
        variable: N -> %;
} == add {
        vars: List String == ["x", "y"];
        Rep == PrimitiveArray N;
        n ==> 2;
        import from Rep, N;
        (p: TW) << (x: %): TW == {
                p << vars.1 << "^" << rep(x).1 << "*";
                p << vars.2 << "^" << rep(x).2;
        }
        names(): List String == vars;
        variable(i: N): % == {
                error << "VAR: " << i << nl;
                z: Rep := new n;
                z.i := 1;
                per z
        }
}

define CalixPolynomialCategory(
    T: with {
        <<: (TW, %) -> TW;
        names: () -> List String;
        variable: N -> %;
    }
): Category == with {
        <<: (TW, %) -> TW;
        coerce: T -> %;
        variable: String -> %;
    default {
        import from T, List String;
        variable(s: String): % == {
                import from N, T;
                for var in names() for i in 1.. repeat {
                        if s = var then return variable(i)::%;
                }               
                error concat("wrong variable name: (", s, ")");
        }
    }
}


CalixOreArrayPolynomials(
    T: with {
        <<: (TW, %) -> TW;
        names: () -> List String;
        variable: N -> %;
    }
): CalixPolynomialCategory(T) with {
        init: N -> ();
} == add {
        Rep == T;
        import from Rep, N;
        coerce(t: T): % == per(t);
        init(i:N): () == {
                error << "INIT:" << i << nl;
                vars: List String := names()$T;
                y: % := variable(vars.1)$%;
                error << "y: " << ":" << y << nl;
                z: % := variable(vars.1);
                error << "z: " << ":" << z << nl;
        }
        (p: TextWriter) << (x: %): TextWriter == p << rep x;
#if CA
        init(0);
#endif
}

import from List String;

main(): () == {
        T == CalixPowerProducts;
        P == CalixOreArrayPolynomials(T);
        import from P,N;
        init(1);
}

main();

