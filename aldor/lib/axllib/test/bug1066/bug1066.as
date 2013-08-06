--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Date: Thu, 18 Apr 96 21:44:30 BST
--* Received: from co.uk (nags8) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA21670; Thu, 18 Apr 96 21:44:31 BST
--* To: ax-bugs
--* Subject: [4][tinfer] equal types aren't

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.6
-- Original bug file name: t.as

#include "axllib"

Foo: with {
	foo: () -> %;
} == add {
	Rep == SingleInteger;
	import from Rep;

	foo(): % == {
		import from SingleInteger;
		y: Rep :=  1;
		for x in 1.. repeat y := y + 1;
		per y
	}
}
