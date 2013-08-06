--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA23492; Mon, 18 Sep 95 13:42:07 BST
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.20007-0@sun3.nsfnet-relay.ac.uk>;
--*           Mon, 18 Sep 1995 13:37:29 +0100
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id OAA08125 
--*           for <ax-bugs@nag.co.uk>; Mon, 18 Sep 1995 14:37:26 +0200
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id OAA27564 
--*           for ax-bugs@nag.co.uk; Mon, 18 Sep 1995 14:37:25 +0200
--* Date: Mon, 18 Sep 1995 14:37:25 +0200
--* Message-Id: <199509181237.OAA27564@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [6] internal compiler error 72

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fo -Q1 nilrec.as
-- Version: 1.1.3
-- Original bug file name: nilrec.as

------------------------------- nilrec.as ----------------------------------
--
-- This compiles "ok" at -Q2 (although this causes runtime problems),
-- but causes an internal compiler error at -Q1:
--
-- % axiomxl -Fo -Q2 nilrec.as
--
-- % axiomxl -Fo -Q1 nilrec.as
-- Compiler bug...Bug: Bad case 72 (line 2066 in file ../src/genc.c).
--

#include "axllib.as"

NilRec(T:Tuple Type): with { rec: % -> Record T } == add {
	macro Rep == Pointer;
	rec(r:%):Record T == rep(r) pretend Record T;
}


Foo(T:Type): with { foo: % -> T } == add {
	macro Rep == NilRec T;
	foo(p:%):T == {
		import from Rep, Record T;
		explode rec rep p;
	}
}
