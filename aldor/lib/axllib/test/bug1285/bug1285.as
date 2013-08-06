--* From mnd@dcs.st-and.ac.uk  Fri Nov 10 15:11:36 2000
--* Received: from server-5.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA13815
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:11:35 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 26750 invoked from network); 10 Nov 2000 15:11:03 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-5.tower-4.starlabs.net with SMTP; 10 Nov 2000 15:11:03 -0000
--* Received: from dcs.st-and.ac.uk (ara3234-ppp [138.251.206.28])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id PAA12107
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:10:38 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id PAA24207
--* 	for ax-bugs@nag.co.uk; Fri, 10 Nov 2000 15:06:47 GMT
--* Date: Fri, 10 Nov 2000 15:06:47 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011101506.PAA24207@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [1][tinfer] segfault with except types

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib exntype0.as
-- Version: 1.1.13(6)
-- Original bug file name: exntype0.as


#include "axllib"

AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- The decoration of the return type of this function with an exception
-- type segfaults the compiler in titdn: missing a tpossRefer. I've now
-- fixed this bug ...
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

