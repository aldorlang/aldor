--* From Manuel.Bronstein@sophia.inria.fr  Mon Sep 25 14:00:09 2000
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id OAA17925
--* 	for <ax-bugs@nag.co.uk>; Mon, 25 Sep 2000 14:00:08 +0100 (BST)
--* Received: by droopix.inria.fr (8.10.0/8.10.0) id e8PD06v29778 for ax-bugs@nag.co.uk; Mon, 25 Sep 2000 15:00:06 +0200
--* Date: Mon, 25 Sep 2000 15:00:06 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200009251300.e8PD06v29778@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] wrong overloaded func called at runtime

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -ginterp -laxllib wrongfunc.as
-- Version: 1.1.12p6
-- Original bug file name: wrongfunc.as

-------------------------- wrongfunc.as -----------------------------
--
-- Executing this should print 1 but prints 0, which means that
-- the wrong 'foo' was called!
--
-- % axiomxl -ginterp wrongfunc.as
-- 0
--

#include "axllib"

macro Z == SingleInteger;

Foo(S:Type): with {
	foo: (S -> ()) -> %;
	foo: (() -> S) -> %;
	value: % -> Z;
} == add {
	Rep == Z;
	import from Rep;
    
	foo(f:S -> ()):%== per 0;
	foo(f:() -> S):%== per 1;
	value(g:%):Z	== rep g;
}

local bar():Z == 22;

local Main():() == {
	import from Z, Foo Z;
	r := foo bar;				-- calls the wrong 'foo'
	print << value r << newline;		-- expects 1
}

Main();
