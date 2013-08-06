--* From bronstei@inf.ethz.ch  Mon Nov 11 17:20:45 1996
--* Received: from nags2.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA10826; Mon, 11 Nov 96 17:20:45 GMT
--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA06028; Mon, 11 Nov 96 17:25:18 GMT
--* Received: from ru8.inf.ethz.ch (bronstei@ru8.inf.ethz.ch [129.132.12.17]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id SAA26404 for <ax-bugs@nag.co.uk>; Mon, 11 Nov 1996 18:18:39 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (from bronstei@localhost) by ru8.inf.ethz.ch (8.7.1/8.7.1) id SAA10746 for ax-bugs@nag.co.uk; Mon, 11 Nov 1996 18:18:38 +0100 (MET)
--* Date: Mon, 11 Nov 1996 18:18:38 +0100 (MET)
--* Message-Id: <199611111718.SAA10746@ru8.inf.ethz.ch>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] default arguments are broken!

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fx defarg.as
-- Version: 1.1.8
-- Original bug file name: defarg.as

----------------------------- defarg.as -------------------------
--
-- Default arguments are seriously broken in 1.1.8:
--
-- % axiomxl -q1 -fx defarg.as
-- % defarg
-- foo() = def
--

#include "axllib"

Foo(x:String == "def"): with { foo:() -> String; } == add { foo():String == x; }

import from Foo("hey I gave a value for the argument!");

print << "foo() = " << foo() << newline;

