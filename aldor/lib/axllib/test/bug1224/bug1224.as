--* From mnd@knockdhu.dcs.st-and.ac.uk  Wed May 31 15:47:45 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id PAA12366
--* 	for <ax-bugs@nag.co.uk>; Wed, 31 May 2000 15:47:43 +0100 (BST)
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id PAA10086
--* 	for ax-bugs@nag.co.uk; Wed, 31 May 2000 15:53:34 +0100
--* Date: Wed, 31 May 2000 15:53:34 +0100
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200005311453.PAA10086@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [6] Confusion with embeddings?

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib bug1224.as
-- Version: 1.1.12p5
-- Original bug file name: iter0.as

#include "axllib"

main():() ==
{
   import from List SingleInteger;

   -- These all work fine.
   print << [ 4 ] << newline;
   print << [ ( 5 ) ] << newline;
   print << [ { 6 } ] << newline;
   print << [ 2, 2, 2 ] << newline;


   -- Here we generate code for a List(Tuple(SingleInteger)) but
   -- access it as a List(SingleInteger).
   print << [ { for i in 1..3 repeat print << i << newline; 2} ] << newline;
}

main();
