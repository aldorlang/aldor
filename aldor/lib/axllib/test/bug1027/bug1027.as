--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA05544; Fri, 22 Sep 95 12:13:20 BST
--* Received: from neptune.ethz.ch by sun3.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.17236-0@sun3.nsfnet-relay.ac.uk>;
--*           Fri, 22 Sep 1995 12:09:24 +0100
--* Received: from ru7.inf.ethz.ch (ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id NAA16953 
--*           for <ax-bugs@nag.co.uk>; Fri, 22 Sep 1995 13:09:20 +0200
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id NAA13998 
--*           for ax-bugs@nag.co.uk; Fri, 22 Sep 1995 13:08:50 +0200
--* Date: Fri, 22 Sep 1995 13:08:50 +0200
--* Message-Id: <199509221108.NAA13998@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [6] Boolean cannot be extended

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.3
-- Original bug file name: bool.as

------------------------------ xbool.as ---------------------------------
--
-- Seems like it's not possible to extend Boolean:
--
-- % axiomxl -M2 bool.as
-- "bool.as", line 26:         t := foo b;
--                     .............^
-- [L26 C14] #1 (Error) There are no suitable meanings for the operator `foo'.
--

#include "axllib.as"

MyType: BasicType with { frombool: Boolean -> % } == SingleInteger add {
	macro Rep == SingleInteger;
	import from Rep;
	frombool(b:Boolean):% == { b => per 1 ; per 0 };
}

MyCat: Category == BasicType with { foo: % -> MyType };

extend Boolean: MyCat == add { foo(x:%):MyType == frombool x };

main():TextWriter == {
	import from SingleInteger, Boolean, MyType;
	b := 2 = 3;
	t := foo b;
	print << t << newline;
}

main();
