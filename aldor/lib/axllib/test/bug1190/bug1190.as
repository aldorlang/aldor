--* From mnd@knockdhu.cs.st-andrews.ac.uk  Mon Feb  7 14:01:01 2000
--* Received: from knockdhu.cs.st-andrews.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id OAA05382
--* 	for <ax-bugs@nag.co.uk>; Mon, 7 Feb 2000 14:00:57 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.cs.st-andrews.ac.uk (8.8.7/8.8.7) id OAA12253
--* 	for ax-bugs@nag.co.uk; Mon, 7 Feb 2000 14:04:43 GMT
--* Date: Mon, 7 Feb 2000 14:04:43 GMT
--* From: mnd <mnd@knockdhu.cs.st-andrews.ac.uk>
--* Message-Id: <200002071404.OAA12253@knockdhu.cs.st-andrews.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][genfoam] Lazy function used before forced in domain init.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See header comments in `textwritx.as'
-- Version: 1.1.12p5 (private edition)
-- Original bug file name: lazybug.txt


--------------------------------------------------------------------------
--==========> Split into two parts: textwritx.as and segv.as <==========--
--------------------------------------------------------------------------




--------------------------------------------------------------------------
------------------------------ textwritx.as ------------------------------
--------------------------------------------------------------------------
-- This file must be compiled -Fao -Fo and inserted into libaxllib.al and
-- libaxllib.a respectively. Then compile and run `segv.as'.
--
-- The bug can be seen by looking at the FOAM generated from this file:
-- examine the `addLevel1' function and notice that the lexical `writer'
-- is lazily-imported *after* it has been used for `cout'.
--
-- % axiomxl -Fao -Fo -Ffm -Q3 -laxllib textwritx.as
-- % ar r $AXIOMXLROOT/lib/libaxllib.al textwritx.ao
-- % ar r $AXIOMXLROOT/lib/libaxllib.a  textwritx.o
-- % axiomxl -Fx -Q3 -laxllib segv.as
-- % ./segv
--
-- Alternatively just drop textwritx.as into the Axllib source directory
-- and update the Makefile so that it becomes part of the library.
--------------------------------------------------------------------------

#include "axllib"


-- Comment out this line to see the version that works.
#assert SHOWBUG


extend TextWriter: with
{
   cout:%;
}
== add
{
   SI ==> SingleInteger;
   import from StandardIO;

   local wrCharacter!(out:OutFile)(c:Character):() ==
      write!(out, c);

   local wrString!(out:OutFile)(s:String, st:SI, lt:SI):SI ==
      write!(out, s, st, lt);

   local locfun(out:OutFile):% ==
      writer(wrCharacter! out, wrString! out);

#if SHOWBUG
   cout:% == locfun(stdout);
#else
   cout:% == writer(wrCharacter! stdout, wrString! stdout);
#endif
}


--------------------------------------------------------------------------
-------------------------------- segv.as ---------------------------------
--------------------------------------------------------------------------
-- Make sure that `textwritx.as' has been compiled and added to Axllib
-- before compiling this file (see the header comment in `textwritx.as'
-- for specific instructions on how to do this).

#include "axllib"

main():() ==
{
   import from TextWriter;
   cout << "This is a test" << newline;
}

main();
