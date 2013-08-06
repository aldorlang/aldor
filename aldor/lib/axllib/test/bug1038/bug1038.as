--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA03122; Sun, 29 Oct 95 15:12:37 GMT
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.11701-1@sun3.nsfnet-relay.ac.uk>;
--*           Sun, 29 Oct 1995 15:08:35 +0000
--* Received: from mendel.inf.ethz.ch (mannhart@mendel.inf.ethz.ch [129.132.12.20]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id QAA28548 
--*           for <ax-bugs@nag.co.uk>; Sun, 29 Oct 1995 16:08:29 +0100
--* From: Niklaus Mannhart <mannhart@ch.ethz.inf>
--* Received: (mannhart@localhost) by mendel.inf.ethz.ch (8.6.10/8.6.10) 
--*           id QAA20741 for ax-bugs@nag.co.uk; Sun, 29 Oct 1995 16:08:29 +0100
--* Date: Sun, 29 Oct 1995 16:08:29 +0100
--* Message-Id: <199510291508.QAA20741@mendel.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [5] constant assignment via function call -> segmentation fault

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: AXIOM-XL version 1.1.4 for SPARC
-- Original bug file name: tmp/bug.as

-- The following program crashes (segmentation fault) but runs fine if
-- I replace pi: Float := ... with pi: Float == ... i.e. make a 
-- constant. 
-- The program crashes if I do the following: local pi: Float == ...;
--

#include "axllib.as"

import from Float;

f(a: Float): Float == {a*a};
pi: Float := 3.14159;  -- local pi: Float == 3.14159; -- crashes also
p : Float == f(pi);
