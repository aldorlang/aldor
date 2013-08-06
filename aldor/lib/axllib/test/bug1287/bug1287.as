--* From mnd@dcs.st-and.ac.uk  Fri Nov 10 15:11:41 2000
--* Received: from server-14.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA13833
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:11:39 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 31616 invoked from network); 10 Nov 2000 15:11:06 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-14.tower-4.starlabs.net with SMTP; 10 Nov 2000 15:11:06 -0000
--* Received: from dcs.st-and.ac.uk (ara3234-ppp [138.251.206.28])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id PAA12114
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:10:42 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id PAA24221
--* 	for ax-bugs@nag.co.uk; Fri, 10 Nov 2000 15:07:58 GMT
--* Date: Fri, 10 Nov 2000 15:07:58 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011101507.PAA24221@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [1][tinfer] error for valid program

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib exntype1.as
-- Version: 1.1.13(7)
-- Original bug file name: exntype1.as


#include "axllib"

define AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- Having fixed the segfault bug with this code, we now get a type error.
-- The compiler claims that "throw AnError" does not satisfy the context
-- Exit throw RuntimeException. The user guide is ambiguous about this ...
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

