--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA00658; Sat, 23 Sep 95 23:42:39 BST
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.28970-0@sun3.nsfnet-relay.ac.uk>;
--*           Sat, 23 Sep 1995 23:38:53 +0100
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id AAA20886 
--*           for <ax-bugs@nag.co.uk>; Sun, 24 Sep 1995 00:38:51 +0200
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id AAA20228 
--*           for ax-bugs@nag.co.uk; Sun, 24 Sep 1995 00:38:50 +0200
--* Date: Sun, 24 Sep 1995 00:38:50 +0200
--* Message-Id: <199509232238.AAA20228@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [1] coercion Integer -> SingleInteger broken

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx badint.as
-- Version: 1.1.3
-- Original bug file name: badint.as

-----------------------------  badint.as ----------------------------------
--
-- Coercing a SingleInteger to Integer and back produces a different number:
--
-- axiomxl -Fx badint.as
-- badint
-- a,b,c = 1551877902,1551877902,228600
--

#include "axllib.as"

import from SingleInteger, Integer;

a:SingleInteger := 1551877902;    -- smaller than max()@SingleInteger
b:Integer := a::Integer;
c:SingleInteger := b::SingleInteger;

print << "a,b,c = " << a << "," << b << "," << c << newline;

