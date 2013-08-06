--* From adk@epcc.ed.ac.uk  Tue Sep  5 19:37:49 2000
--* Received: from e450.epcc.ed.ac.uk (e450.epcc.ed.ac.uk [129.215.56.230])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id TAA15131
--* 	for <ax-bugs@nag.co.uk>; Tue, 5 Sep 2000 19:37:48 +0100 (BST)
--* Received: from maxinst.epcc.ed.ac.uk (maxinst [129.215.63.6])
--* 	by e450.epcc.ed.ac.uk (8.9.1/8.9.1) with ESMTP id TAA09761;
--* 	Tue, 5 Sep 2000 19:37:13 +0100 (BST)
--* From: Tony Kennedy <adk@epcc.ed.ac.uk>
--* Received: (from adk@localhost)
--* 	by maxinst.epcc.ed.ac.uk (8.9.1/8.9.1) id TAA23727;
--* 	Tue, 5 Sep 2000 19:37:11 +0100 (BST)
--* Date: Tue, 5 Sep 2000 19:37:11 +0100 (BST)
--* Message-Id: <200009051837.TAA23727@maxinst.epcc.ed.ac.uk>
--* To: adk@ph.ed.ac.uk, ax-bugs@nag.co.uk
--* Subject: [4] hash$Integer never returns

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Ginterp hashint.as
-- Version: 1.1.12p2
-- Original bug file name: hashint.as

#include "axllib"
#pile

test1(): () == 
  i: SingleInteger == 42
  print . "hash(~a) = ~a~n" (<< i, << hash i)

test2(): () == 
  import from SingleInteger
  i: Integer == 42
  print . "hash(~a) = ~a~n" (<< i, << hash i)

test1()
test2()
