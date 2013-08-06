--* From mnd@knockdhu.dcs.st-and.ac.uk  Tue Mar  7 12:31:19 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id MAA23234
--* 	for <ax-bugs@nag.co.uk>; Tue, 7 Mar 2000 12:31:03 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id MAA18568
--* 	for ax-bugs@nag.co.uk; Tue, 7 Mar 2000 12:36:24 GMT
--* Date: Tue, 7 Mar 2000 12:36:24 GMT
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200003071236.MAA18568@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][other=inline-optimiz] Not inlining `print'

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source
-- Version: 1.1.12p5
-- Original bug file name: inline00.as


-- Compile with -Ffm -Q5 and note that the `print' export from TextWriter
-- doesn't get inlined. It is left as a lexical corresponding to the
-- function representing textwriters. This function is applied to get
-- each time we want to print something.
--
-- We ought to be able to inline `print' and obtain code that is in terms
-- of the fputss and fputc functions.

#include "axllib"


foo(flag:Boolean):() ==
{
   if (flag) then
   {
      if (flag) then
         print << "true" << newline;
   }
}



