--* From: Peter Broadbery <peterb@uk.co.nag>
--* Date: Tue, 14 Nov 95 17:16:20 GMT
--* Received: from co.uk (nags8) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA01558; Tue, 14 Nov 95 17:16:20 GMT
--* To: ax-bugs@uk.co.nag
--* Subject: [2] Inlining defaults when domain defined w/out a with

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -O sbug.as
-- Version: 1.1.4
-- Original bug file name: sbug.as

#include "axllib"

define C1: Category == with {
	foo: () -> DoubleFloat;
	default {
		foo(): DoubleFloat == 1.2;
	}
}


D1(d: DoubleFloat): C1 == {
	print<< "XX: " << d << newline;
	add {
		foo(): DoubleFloat == d;
	}
}
import from DoubleFloat;
import from D1(2.2);
t1(): () == {
	print << foo() << newline;
	print << foo() << newline;
}

t1();

