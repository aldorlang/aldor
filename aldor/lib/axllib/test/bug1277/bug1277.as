--* From mnd@dcs.st-and.ac.uk  Thu Nov  9 12:54:10 2000
--* Received: from server-12.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id MAA02883
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 12:54:06 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 11935 invoked from network); 9 Nov 2000 12:53:33 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-12.tower-4.starlabs.net with SMTP; 9 Nov 2000 12:53:33 -0000
--* Received: from dcs.st-and.ac.uk (knockdhu [138.251.206.239])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id MAA29191
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 12:53:12 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id MAA31060
--* 	for ax-bugs@nag.co.uk; Thu, 9 Nov 2000 12:54:26 GMT
--* Date: Thu, 9 Nov 2000 12:54:26 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011091254.MAA31060@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][runtime] has questions get unexpected answers

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -ginterp hasbug.as
-- Version: 1.1.13(0)
-- Original bug file name: hasbug.as


#include "axllib"
SI ==> SingleInteger;

FooCat(R:Record(lo:SI, hi:SI)):Category == with {
   foo: () -> SI;
   bar: () -> SI;
}
 
FooDom(R:Record(lo:SI, hi:SI)):FooCat(R) == add {
   foo():SI == R.lo;
   bar():SI == R.hi;
}

import from SingleInteger, Record(lo:SI, hi:SI);
import from FooDom([42, 21]);
 
print << "foo() = " << foo() << ", bar() = " << bar() << newline;
 
local t1:Boolean := FooDom([42, 21]) has FooCat([42, 21]);
local t2:Boolean := FooDom([42, 21]) has FooCat([52, 93]);
print << "FooDom([42, 21]) has FooCat([42, 21]) = " << t1 << newline;
print << "FooDom([42, 21]) has FooCat([52, 93]) = " << t2 << newline;
