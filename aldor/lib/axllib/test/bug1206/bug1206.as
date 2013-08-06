--* From S.J.Thompson@ukc.ac.uk  Fri May  5 14:42:03 2000
--* Received: from mercury.ukc.ac.uk (mercury.ukc.ac.uk [129.12.21.10])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id OAA07426
--* 	for <ax-bugs@nag.co.uk>; Fri, 5 May 2000 14:42:02 +0100 (BST)
--* Received: from myrtle.ukc.ac.uk ([129.12.3.176])
--* 	by mercury.ukc.ac.uk with esmtp (Exim 2.12 #1)
--* 	id 12niOK-0003Y5-00
--* 	for ax-bugs@nag.co.uk; Fri, 5 May 2000 14:43:40 +0100
--* Received: from sjt by myrtle.ukc.ac.uk with local (Exim 2.12 #2)
--* 	id 12niOJ-0006Ke-00
--* 	for ax-bugs@nag.co.uk; Fri, 5 May 2000 14:43:39 +0100
--* To: ax-bugs@nag.co.uk
--* Subject: [5] Where fails to pick scope a function's formal parameters
--* Message-Id: <E12niOJ-0006Ke-00@myrtle.ukc.ac.uk>
--* From: "S.J.Thompson" <S.J.Thompson@ukc.ac.uk>
--* Date: Fri, 5 May 2000 14:43:39 +0100

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp wheretes.as
-- Version: AXIOM-XL version 1.1.10b for SPARC [Solaris: GCC]
-- Original bug file name: /nfs/myrtle/d11/home/cur/sjt/euterpe/Aldor/Examples/wheretes.as

--+ 
--+ -- wheretes.as 7.4.00
--+ 
--+ -- Testing where in Aldor.
--+ 
--+ #include "axllib"
--+ 
--+ import from Integer;		-- to use Integer
--+ 
--+ -- It is apparent that where doesn't include in its environment
--+ -- the formal parameters of the function in whose RHS it is
--+ -- contained. Note that (from fun2) these variables are in scope
--+ -- in the RHS itself!
--+ 
--+ fun1(n:Integer):Integer 
--+     == m+3 where { m:Integer==n+2 ;};
--+ 
--+ fun2(n:Integer):Integer 
--+     == (n+2)+3;

-- wheretes.as 7.4.00

-- Testing where in Aldor.

#include "axllib"

import from Integer;		-- to use Integer

-- It is apparent that where doesn't include in its environment
-- the formal parameters of the function in whose RHS it is
-- contained. Note that (from fun2) these variables are in scope
-- in the RHS itself!

fun1(n:Integer):Integer 
    == m+3 where { m:Integer==n+2 ;};

fun2(n:Integer):Integer 
    == (n+2)+3;
