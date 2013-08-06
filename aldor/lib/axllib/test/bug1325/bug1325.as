--* From hemmecke@risc.uni-linz.ac.at  Mon Sep  3 17:31:46 2001
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id RAA07026
--* 	for <ax-bugs@nag.co.uk>; Mon, 3 Sep 2001 17:31:41 +0100 (BST)
--* From: hemmecke@risc.uni-linz.ac.at
--* Received: (qmail 3396 invoked from network); 3 Sep 2001 09:24:30 -0000
--* Received: from 2.star-private-mail-12.star.net.uk (HELO smtp-in-2.star.net.uk) (10.200.12.2)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 3 Sep 2001 09:24:30 -0000
--* Received: (qmail 24812 invoked from network); 3 Sep 2001 09:24:30 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-2.star.net.uk with SMTP; 3 Sep 2001 09:24:30 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 2050 invoked from network); 3 Sep 2001 09:21:48 -0000
--* Received: from kernel.risc.uni-linz.ac.at (193.170.37.225)
--*   by server-10.tower-17.messagelabs.com with SMTP; 3 Sep 2001 09:21:48 -0000
--* Received: from enceladus.risc.uni-linz.ac.at (hemmecke@thea.risc.uni-linz.ac.at [193.170.38.91])
--* 	by kernel.risc.uni-linz.ac.at (8.11.6/8.11.6) with ESMTP id f839OP922494;
--* 	Mon, 3 Sep 2001 11:24:25 +0200
--* Message-ID: <XFMail.20010903112421.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.4.4 on Linux
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* Date: Mon, 03 Sep 2001 11:24:21 +0200 (CEST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [1]compiler crash with apply and Generator

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Q3 xxx.as
-- Version: Aldor version 1.1.13p1(5) for LINUX(glibc2.1)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 29-AUG-2001
-- Aldor version 1.1.13p1(5) for LINUX(glibc2.1)
-- Subject: compiler crash with apply and Generator

-- I compiled the program on a Compaq Armada V300 laptop. The compiler
-- crashed with the command.
--   axiomxl -V -Q3 xxx.as  

-- Also -q2 crashes while -q1 is OK.

-- thea:/home/hemmecke>uname -a
-- Linux thea 2.2.17 #1 Thu Feb 8 16:57:22 CET 2001 i686 unknown

#include "axllib"

macro {
        CRT == Cross(R, T);
        I == SingleInteger;
}

XCalixPairArray(R: Type, T: Type): with {
        apply: (%, I) -> (R, T);
} == add {
        Rep == CRT;
        import from Rep, R, T;
        apply(x: %, i: I): (R, T) == {crt: CRT := rep x}
}

XCalixArrayPolynomials(
    R: Type,
    T: Type
): with {} == add {
        Rep == XCalixPairArray(R, T);
        import from Rep, SingleInteger;
        monomials(x: %): Generator CRT == {
                (rep x). i for i in 1 .. 4;
        }
}


