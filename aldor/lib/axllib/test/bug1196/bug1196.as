--* From mnd@knockdhu.dcs.st-and.ac.uk  Mon Mar  6 16:38:38 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id QAA15616
--* 	for <ax-bugs@nag.co.uk>; Mon, 6 Mar 2000 16:38:32 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id QAA07221
--* 	for ax-bugs@nag.co.uk; Mon, 6 Mar 2000 16:44:06 GMT
--* Date: Mon, 6 Mar 2000 16:44:06 GMT
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200003061644.QAA07221@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Embeddings fail in presence of `pretend'

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Ffm emb00.as
-- Version: 1.1.12p5 (personal edition)
-- Original bug file name: emb00.as


#include "axllib"

MyString:with { foo: % -> %; } == add
{
   Rep == String;

   foo(x:%):% ==
   {
      local r:String;

      -- This line cause the compiler to invent `local r:Rep' instead
      -- of using the `r:String' that we already declared.
      r := rep x;

      x;
   }
}

