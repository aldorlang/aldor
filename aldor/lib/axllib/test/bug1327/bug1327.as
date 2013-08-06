--* From mnd@knockdhu.dcs.st-and.ac.uk  Tue Sep 18 14:34:51 2001
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id OAA01532
--* 	for <ax-bugs@nag.co.uk>; Tue, 18 Sep 2001 14:34:50 +0100 (BST)
--* Received: (qmail 21195 invoked from network); 18 Sep 2001 13:34:20 -0000
--* Received: from 4.star-private-mail-12.star.net.uk (HELO smtp-in-4.star.net.uk) (10.200.12.4)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 18 Sep 2001 13:34:20 -0000
--* Received: (qmail 18340 invoked from network); 18 Sep 2001 13:34:20 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-4.star.net.uk with SMTP; 18 Sep 2001 13:34:20 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 10610 invoked from network); 18 Sep 2001 13:32:44 -0000
--* Received: from host213-122-185-176.btinternet.com (HELO knockdhu.dcs.st-and.ac.uk) (213.122.185.176)
--*   by server-7.tower-17.messagelabs.com with SMTP; 18 Sep 2001 13:32:44 -0000
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.11.2/8.11.2) id f8IDbiR11846
--* 	for ax-bugs@nag.co.uk; Tue, 18 Sep 2001 14:37:44 +0100
--* Date: Tue, 18 Sep 2001 14:37:44 +0100
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200109181337.f8IDbiR11846@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][other=gen0RtTypeHash] name and type hash collisions

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -Q0 -grun -laxllib bug1327.as
-- Version: 1.0.-1(6)
-- Original bug file name: bug1327.as


-- This is a resubmission of bug 1188 which had previously been fixed (see
-- AxiomXL 1.1.13(18)). Unfortunately the fix induces similar problems in
-- domains with deep type trees but with no obvious cause. Edit 18 has been
-- removed since it is easier to explain to users what this bug is and it
-- is just as easy for them to spot it themselves.
--
-- Ideally the fix for this bug will follow that of edit 18 except that the
-- type hash codes must not collide so easily.
--
-- Note that the bug is due to the fact that we collect all the types used
-- in a -> constructor and hash them together. Strictly-speaking we ought
-- to hash the tuple of argument types, hash the tuple of result types and
-- then treat -> as a binary operator (which it is).
--
-- Related bugs are 1260 and 1272.
--
-- Note that inlining hides the problem because we search for inlinees by
-- Syme not by hash code.

#include "axllib"

SI ==> SingleInteger;

Foo: with
{
   foo: SI -> ();
   foo: () -> SI;
}
== add
{
   foo(x:SI):() == { print << "foo(" << x << ")" << newline; }
   foo():SI == { print << "foo()" << newline; 42; }
}

main():() == {
   import from SI, Foo;
   foo(42);
   foo();
}

main();

