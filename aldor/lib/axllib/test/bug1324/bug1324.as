--* From mnd@knockdhu.dcs.st-and.ac.uk  Mon Sep  3 15:49:39 2001
--* Received: from mail-delivery-1.star.net.uk (welly-1.star.net.uk [195.216.16.165])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA01254
--* 	for <ax-bugs@nag.co.uk>; Mon, 3 Sep 2001 15:49:38 +0100 (BST)
--* Received: (qmail 15484 invoked from network); 3 Sep 2001 12:02:28 -0000
--* Received: from 4.star-private-mail-12.star.net.uk (HELO smtp-in-4.star.net.uk) (10.200.12.4)
--*   by delivery-1.star-private-mail-4.star.net.uk with SMTP; 3 Sep 2001 12:02:28 -0000
--* Received: (qmail 26104 invoked from network); 3 Sep 2001 12:02:28 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-4.star.net.uk with SMTP; 3 Sep 2001 12:02:28 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 2509 invoked from network); 3 Sep 2001 11:55:57 -0000
--* Received: from host213-122-120-35.btinternet.com (HELO knockdhu.dcs.st-and.ac.uk) (213.122.120.35)
--*   by server-14.tower-17.messagelabs.com with SMTP; 3 Sep 2001 11:55:57 -0000
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.11.2/8.11.2) id f83C23314644
--* 	for ax-bugs@nag.co.uk; Mon, 3 Sep 2001 13:02:03 +0100
--* Date: Mon, 3 Sep 2001 13:02:03 +0100
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200109031202.f83C23314644@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [5] Compiler crash with apply and Generator

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -Q3 ralfxxx.as
-- Version: 1.0.-1(3)
-- Original bug file name: ralfxxx.as

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
                apply(rep x, i) for i in 1 .. 4;
        }
}


