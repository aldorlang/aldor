--* From mnd@knockdhu.cs.st-andrews.ac.uk  Wed Jan 19 15:10:38 2000
--* Received: from knockdhu.cs.st-andrews.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id PAA27405
--* 	for <ax-bugs@nag.co.uk>; Wed, 19 Jan 2000 15:10:33 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.cs.st-andrews.ac.uk (8.8.7/8.8.7) id PAA22656
--* 	for ax-bugs@nag.co.uk; Wed, 19 Jan 2000 15:12:57 GMT
--* Date: Wed, 19 Jan 2000 15:12:57 GMT
--* From: mnd <mnd@knockdhu.cs.st-andrews.ac.uk>
--* Message-Id: <200001191512.PAA22656@knockdhu.cs.st-andrews.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [4][other=hashing] Hash code collisions

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.12p5
-- Original bug file name: hashbug.as

--+ 
--+ The types used in a function signature are all collected together and
--+ the resulting tuple is used to create a hash code. This means that the
--+ following two functions are assumed to be the same:
--+ 
--+    foo: (T1, T2, T3) -> ();
--+    foo: (T1, T2) -> T3;
--+ 
--+ because their signatures are both (T1, T2, T3). The compiler generate code
--+ for both functions but will only ever call the first one.

#include "axllib"

SI ==> SingleInteger;

Foo: with
{
   foo: SI -> ();
   foo: () -> SI;
}
== add
{
   foo(x:SI):() == { print << "foo(" << x << ")" << newline; }
   foo():SI == { print << "foo()" << newline; 42; }
}

import from SI, Foo;
foo(42);
foo();
