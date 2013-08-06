--* From Manuel.Bronstein@sophia.inria.fr  Thu Apr 22 18:06:15 1999
--* Received: from nirvana.inria.fr (bmanuel@nirvana.inria.fr [138.96.48.30])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id SAA24146
--* 	for <ax-bugs@nag.co.uk>; Thu, 22 Apr 1999 18:06:06 +0100 (BST)
--* Received: by nirvana.inria.fr (8.8.8/8.8.5) id TAA18098 for ax-bugs@nag.co.uk; Thu, 22 Apr 1999 19:05:20 +0200
--* Date: Thu, 22 Apr 1999 19:05:20 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199904221705.TAA18098@nirvana.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [6] 'local' keyword causes runtime error

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q1 -fx hashbug.as
-- Version: 1.1.12
-- Original bug file name: hashbug.as

-------------------------- hashbug.as ------------------------
--
-- This causes the following runtime error:
--
-- % axiomxl -fx -q1 hashbug.as
-- % hashbug
-- hash value read on incomplete domain
-- Unhandled Exception: RuntimeError(??)
-- Error: ???.
--

#include "axllib"

Foo(T:BasicType):with { foo: () -> % } == add {
	-- the bug disappears if the 'local' keyword is removed below (why?)
	local Bar:BasicType == add {
		macro Rep == T;
		import from Rep;
		(a:%) = (b:%):Boolean == rep(a) = rep(b);
		sample:% == per(sample$T);
		(p:TextWriter) << (x:%):TextWriter == p << rep x;
	}

	macro Rep == List Bar;
	import from Rep;
	foo():%	== per empty();
	sample:% == foo();
	(p:TextWriter) << (x:%):TextWriter == p << rep x;
}

main():() == {
	import from Foo SingleInteger;
	foo();
}

main();
