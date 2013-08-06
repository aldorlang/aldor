--* From mnd@dcs.st-and.ac.uk  Fri Nov 10 15:11:41 2000
--* Received: from server-14.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA13839
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:11:40 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 31644 invoked from network); 10 Nov 2000 15:11:08 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-14.tower-4.starlabs.net with SMTP; 10 Nov 2000 15:11:08 -0000
--* Received: from dcs.st-and.ac.uk (ara3234-ppp [138.251.206.28])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id PAA12110
--* 	for <ax-bugs@nag.co.uk>; Fri, 10 Nov 2000 15:10:40 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id PAA24249
--* 	for ax-bugs@nag.co.uk; Fri, 10 Nov 2000 15:09:17 GMT
--* Date: Fri, 10 Nov 2000 15:09:17 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011101509.PAA24249@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] segfault with exception types

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib exntype3.as
-- Version: 1.1.13(7)
-- Original bug file name: exntype3.as


#include "axllib"

define AnError:RuntimeException with == add
{
   name():String == "An error has occurred";

   printError(t:TextWriter):() ==
      t << name() << newline;
}


-- In an attempt to prevent the compiler from complaining about the type
-- of a "throw" statement, here we force the function to have a void value
-- and are rewarded with a segfault.
foo():() throw (RuntimeException) ==
{
   print << "Here goes ..." << newline;
   throw AnError;
   ();
}


try foo() catch E in { true => print << "          ... BOOM!" << newline; }

print << "That's all folks!" << newline;

