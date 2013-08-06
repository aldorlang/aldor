--* From bronstei@inf.ethz.ch  Wed Dec 18 14:43:29 1996
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA13784; Wed, 18 Dec 96 14:43:29 GMT
--* Received: from inf.ethz.ch (root@neptune.ethz.ch [129.132.10.10])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with SMTP
--* 	  id OAA03284 for <ax-bugs@nag.co.uk>; Wed, 18 Dec 1996 14:44:05 GMT
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id PAA22880 for <ax-bugs@nag.co.uk>; Wed, 18 Dec 1996 15:43:31 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id PAA19621 for ax-bugs@nag.co.uk; Wed, 18 Dec 1996 15:43:30 +0100
--* Date: Wed, 18 Dec 1996 15:43:30 +0100
--* Message-Id: <199612181443.PAA19621@ru7.inf.ethz.ch>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] runtime memory problem

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.8a
-- Original bug file name: bigmem.as

------------------------- bigmem.as ------------------------
--
-- This report illustrates the problems with the runtime as soon
-- as significant memory gets allocated. On a dec alpha with 256Mb RAM
-- (and much more page space), the following 72Mb heap request causes
-- runtime seg fault at every run. On the other hand, reducing n to 900
-- makes everything work ok each time! This happens at all opt level,
-- you may have to tune the n and m parameters in order to reproduce it.
--
-- Another problem is that this program should allocate 72Mb on a 64bit
-- machine, but it seems to have allocated 131Mb by the time it seg faults.
-- Are use using a double heap?
--
-- % axiomxl -fx bigmem.as
-- bigmem
-- Segmentation fault
--

#include "axllib"

macro {
	SI == SingleInteger;
	PA == PrimitiveArray;
	n == 1000;
	N == 18000;
}

import from SI, PA SI, PA PA SI;

mat:PA PA SI := new n;
for i in 1..n repeat mat.i := new(N, 0);

