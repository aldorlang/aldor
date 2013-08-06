--* From mnd@dcs.st-and.ac.uk  Tue Jul 17 14:18:32 2001
--* Received: from mail.london-1.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id OAA13141
--* 	for <ax-bugs@nag.co.uk>; Tue, 17 Jul 2001 14:18:31 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 5382 invoked from network); 17 Jul 2001 13:15:17 -0000
--* Received: from host213-122-54-94.btinternet.com (HELO dcs.st-and.ac.uk) (213.122.54.94)
--*   by server-2.tower-4.starlabs.net with SMTP; 17 Jul 2001 13:15:17 -0000
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.11.0/8.11.0) id f6HDKJI27713
--* 	for ax-bugs@nag.co.uk; Tue, 17 Jul 2001 14:20:19 +0100
--* Date: Tue, 17 Jul 2001 14:20:19 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200107171320.f6HDKJI27713@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][tinfer] embeddings fail in the precence of restrict

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -DSEGV -laxllib bug1318.as
-- Version: 1.1.13(31)
-- Original bug file name: bug1318.as

--> testerrs -DSEGV

-- Based on bug1295.

#include "axllib"
SI ==> SingleInteger;
T ==> Tuple;
L ==> List;
import from T(SI),SI,L(T(SI));

------------------------------------------------------------------------
-- The following three values ought to be equivalent:
--    [1@T(SI), 12@T(SI)]@L(T(SI))
--    [1@SI,    12@SI   ]@L(T(SI))
--    [(1,      12)     ]@L(T(SI))
--
-- However, while the last two produce valid code (list of two elements
-- each a singleton tuple), the first does not. The problem seems to be
-- that when we restrict 1@T(SI) we fail to realise that we must create
-- the embedding from SI -> T(SI).
------------------------------------------------------------------------

#if SEGV
l:L(T(SI)) := [1@Tuple(SI),12@Tuple(SI)]; -- Equivalent to definition below
#elseif OKAY
l:L(T(SI)) := [1@SI,12@SI]; -- Equivalent to definition below
#else
l:L(T(SI)) := [(1,12)]; -- This is a list of two one-element tuples!
#endif

print << "[" << newline;
for e in l repeat {
   print << "   (";
   if (length(e)>0) then print << element(e,1);
   for i in 2..length(e) repeat
     print << ", " <<element(e,i);
   print << ")" << newline;
}
print << "]" << newline;

