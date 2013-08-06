--* From mnd@knockdhu.dcs.st-and.ac.uk  Mon Mar 27 17:14:08 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id RAA19198
--* 	for <ax-bugs@nag.co.uk>; Mon, 27 Mar 2000 17:14:06 +0100 (BST)
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id RAA04181
--* 	for ax-bugs@nag.co.uk; Mon, 27 Mar 2000 17:18:33 +0100
--* Date: Mon, 27 Mar 2000 17:18:33 +0100
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200003271618.RAA04181@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Fun and games with 1

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.12p5 (private edition)
-- Original bug file name: oops.as


#include "axllib"

-- Try with and without optimisation ;)

main():() ==
{
   import from SingleInteger;

   for 1 in 1..100 repeat
      print << 1 << newline;
}

main();
