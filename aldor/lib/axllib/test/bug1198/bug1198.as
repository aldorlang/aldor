--* From mnd@knockdhu.dcs.st-and.ac.uk  Tue Mar  7 12:29:46 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id MAA23160
--* 	for <ax-bugs@nag.co.uk>; Tue, 7 Mar 2000 12:29:34 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id MAA18504
--* 	for ax-bugs@nag.co.uk; Tue, 7 Mar 2000 12:35:07 GMT
--* Date: Tue, 7 Mar 2000 12:35:07 GMT
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200003071235.MAA18504@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][other=jflow-optimiz] Repeated tests not eliminated

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source
-- Version: 1.1.12p5
-- Original bug file name: jflow00.as


-- Compile with -Ffm -Q5 and then look at the FOAM for `foo'. Note that
-- the `if' test is repeated even though the second test must succeed.

#include "axllib"


foo(flag:Boolean):() ==
{
   if (flag) then
   {
      if (flag) then
         print << "true" << newline;
   }
}



