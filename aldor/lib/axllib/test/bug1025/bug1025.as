--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA23651; Mon, 18 Sep 95 14:14:30 BST
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.21456-0@sun3.nsfnet-relay.ac.uk>;
--*           Mon, 18 Sep 1995 14:10:27 +0100
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id PAA09304 
--*           for <ax-bugs@nag.co.uk>; Mon, 18 Sep 1995 15:10:25 +0200
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id PAA27897 
--*           for ax-bugs@nag.co.uk; Mon, 18 Sep 1995 15:10:24 +0200
--* Date: Mon, 18 Sep 1995 15:10:24 +0200
--* Message-Id: <199509181310.PAA27897@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [4] default parameters still broken

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx defparam.as
-- Version: 1.1.3
-- Original bug file name: defparam.as

----------------------------- defparam.as ----------------------------------
--
-- Looks like default parameters are still broken in 1.1.3:
--
-- % axiomxl -Fx defparam.as
-- % defparam
-- Looking in Foo(Integer, ??) for - with code 318693034
-- Export not found
--

#include "axllib.as"

macro Z == Integer;

Foo(R:Ring, avar:String == "x"):Ring == add {
	macro Rep == R;

	import from Z, Rep;

	0:% == foo 0;
	1:% == foo 1;
	(port:TextWriter) << (p:%):TextWriter   == port;
	coerce(n:SingleInteger):% == n::Z::%;
	coerce(n:Z):% == foo(n::R);
	(p:%)^(n:Z):% == p;
	foo(c:R):% == per c;
	-(p:%):% == p;
	(x:%) = (y:%):Boolean == false;
	(p:%) + (q:%):% == p;
	(p:%) * (q:%):% == p;
}

macro F == Foo(Z, "x");

bar():Boolean == {
	import from Z, F;
	a:F := 1;
	a - 1 = 0
}

bar();
