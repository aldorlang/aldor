--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA27521; Tue, 19 Mar 96 17:32:48 GMT
--* Received: from vinci.inf.ethz.ch (bronstei@vinci.inf.ethz.ch [129.132.12.46]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id SAA03022 for <ax-bugs@nag.co.uk>; Tue, 19 Mar 1996 18:27:54 +0100
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id SAA10898 for ax-bugs@nag.co.uk; Tue, 19 Mar 1996 18:27:53 +0100
--* Date: Tue, 19 Mar 1996 18:27:53 +0100
--* Message-Id: <199603191727.SAA10898@vinci.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [9] runtime segfault on dubious code

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q1 -fx segfault.as
-- Version: 1.1.6
-- Original bug file name: segfault.as

--------------------------- segfault.as -----------------------
--
-- From santas@inf.ethz.ch Thu Nov 24 18:13:44 1994
-- Compiles but seg faults at runtime
-- I'm not sure whether this is supposed to be legal A# though (MB)
--
-- % axiomxl -q1 -fx segfault.as
-- % segfault
-- Segmentation fault
--

#include "axllib"

import from SingleInteger;

i:SingleInteger := 1;

D1:with{x:SingleInteger} == add{x == {free i; i}};

f():SingleInteger == {
	free i;
	while i <= 10 repeat {
		D == D1;
		i := i + x$D
	}
	i
}

f();

