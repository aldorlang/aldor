--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA03326; Wed, 20 Dec 95 14:16:12 GMT
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <msg.25164-0@sun3.nsfnet-relay.ac.uk>;
--*           Wed, 20 Dec 1995 14:11:01 +0000
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id PAA14463 
--*           for <ax-bugs@nag.co.uk>; Wed, 20 Dec 1995 15:10:33 +0100
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id PAA23198 
--*           for ax-bugs@nag.co.uk; Wed, 20 Dec 1995 15:10:32 +0100
--* Date: Wed, 20 Dec 1995 15:10:32 +0100
--* Message-Id: <199512201410.PAA23198@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [4] default argument cause runtime export not found

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx notfound.as
-- Version: 1.1.5
-- Original bug file name: notfound.as

------------------------------ notfound.as ---------------------------------
--
-- Default arguments strike again:
--
-- % axiomxl -Fx notfound.as
-- % notfound
-- Looking in SingleInteger for 1 with code 0
-- Export not found
--

#include "axllib"

Foo(R:Ring, S:Type): with { foo: (S, d:R == 1) -> R } == add {
	foo(s:S, d:R == 1):R == d;
}

import from SingleInteger, Foo(SingleInteger, SingleInteger);

foo 120;
