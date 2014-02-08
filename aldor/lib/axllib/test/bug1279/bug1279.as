--* From mnd@dcs.st-and.ac.uk  Thu Nov  9 13:21:04 2000
--* Received: from server-10.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id NAA03286
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:21:03 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 19890 invoked from network); 9 Nov 2000 13:20:33 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-10.tower-4.starlabs.net with SMTP; 9 Nov 2000 13:20:33 -0000
--* Received: from dcs.st-and.ac.uk (knockdhu [138.251.206.239])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id NAA00411
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:20:08 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id NAA31231
--* 	for ax-bugs@nag.co.uk; Thu, 9 Nov 2000 13:21:22 GMT
--* Date: Thu, 9 Nov 2000 13:21:22 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011091321.NAA31231@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [5][tinfer] type of sequence is wrongly tinfered

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib exnbug.as
-- Version: 1.1.13(0)
-- Original bug file name: exnbug.as


#include "axllib"

main(n:SingleInteger, m:SingleInteger):SingleInteger ==
{
   local msg:String;


   -- This is okay because throw(RuntimeError()) has type Exit which
   -- satisfies any type including SingleInteger.
   (m > n) =>
      throw RuntimeError("m > n");


   -- This is also okay for the same reasons. However, the type checker
   -- seems to use "msg" as the return value which must be examined to
   -- type check the value of this sequence.
   (n < m) =>
   {
      msg := "n < m";
      throw RuntimeError(msg);
   }

   42;
}

import from GeneralAssert;
import from SingleInteger;
assertFail((): () +-> main(21, 64));


