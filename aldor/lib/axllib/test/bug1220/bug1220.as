--* From Manuel.Bronstein@sophia.inria.fr  Tue May 30 16:41:14 2000
--* Received: from droopix.inria.fr (IDENT:root@droopix.inria.fr [138.96.111.4])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id QAA29641
--* 	for <ax-bugs@nag.co.uk>; Tue, 30 May 2000 16:40:40 +0100 (BST)
--* Received: by droopix.inria.fr (8.10.0/8.10.0) id e4UFTxD26494 for ax-bugs@nag.co.uk; Tue, 30 May 2000 17:29:59 +0200
--* Date: Tue, 30 May 2000 17:29:59 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200005301529.e4UFTxD26494@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] extend not working properly

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q1 -ginterp earlybind.as
-- Version: 1.1.12p4
-- Original bug file name: earlybind.as

--------------------------- earlybind.as ----------------------------
--
-- foo() is defined by extension properly, but bar() remains bound
-- to the earlier definition of foo(). This does not seem to be
-- related to the optimizer (happens at -q1).
--
--
-- % axiomxl -ginterp -q1 earlybind.as 
-- foo()$Foo = 0
-- bar(1)$Foo = #0 140463328 in <foo> at unit [earlybind]
-- #1 d44 in <bar> at unit [earlybind]
-- #2 13a17 in <lazyGetter> at unit [runtime]
-- #3 a20 in <earlybind> at unit [earlybind]
-- ...
-- Unhandled Exception: RuntimeError(??)
-- User error: Reached a "never"
-- 

#include "axllib"

macro I == SingleInteger;

Foo: BasicType with {
	foo: () -> %;
	bar: I -> %;
	baz: I -> %;
} == I add {
	foo():% == never;             -- to be extended
	bar(n:I):% == foo();          -- bound too early to never!
	baz(n:I):% == n pretend %;
}

extend Foo: with {} == add { foo():% == { import from I; baz 0 } }

import from Foo, I;

print << "foo()$Foo = " << foo() << newline;
print << "bar(1)$Foo = " << bar 1 << newline;

