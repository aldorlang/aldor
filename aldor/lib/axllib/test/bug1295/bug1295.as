--* From bill@scl.csd.uwo.ca  Fri Jan 26 20:45:25 2001
--* Received: from server-7.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id UAA01579
--* 	for <ax-bugs@nag.co.uk>; Fri, 26 Jan 2001 20:45:24 GMT
--* From: bill@scl.csd.uwo.ca
--* X-VirusChecked: Checked
--* Received: (qmail 19187 invoked from network); 26 Jan 2001 20:43:48 -0000
--* Received: from ptibonum.scl.csd.uwo.ca (129.100.16.102)
--*   by server-7.tower-4.starlabs.net with SMTP; 26 Jan 2001 20:43:48 -0000
--* Message-Id: <200101262044.f0QKiJC06510@medium.scl.csd.uwo.ca>
--* Date: Fri, 26 Jan 2001 15:44:19 -0500
--* To: ax-bugs@nag.co.uk
--* Subject: [5] using List(Tuple(SingleInteger)) aldor thinks there is one to many elements in the list unless the Tuple is first put into a variable

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: Aldor version 1.1.12 for LINUX(glibc)
-- Original bug file name: /scl/people/bill/Bug/demo.as

#include "axllib"
SI ==> SingleInteger;
T ==> Tuple;
L ==> List;
import from T(SI),SI,L(T(SI));

l:L(T(SI)) := [(11,22)]; -- bizzarely, this parses as a list of two items.
for e in l repeat {
  print << length(e) << newline;
  print << element(e,1) << newline
}

l:L(T(SI)) := [(1,2),(3,4)];
for e in l repeat {
  print << length(e) << newline;
  print << element(e, 1)<<", "<<element(e, 2)<<newline;
}

