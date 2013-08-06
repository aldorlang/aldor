--* From Manuel.Bronstein@sophia.inria.fr  Mon Aug  5 17:00:51 2002
--* Received: from welly-2.star.net.uk (welly-2.star.net.uk [195.216.16.189])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id RAA04315
--* 	for <ax-bugs@nag.co.uk>; Mon, 5 Aug 2002 17:00:50 +0100 (BST)
--* Received: (qmail 11901 invoked by uid 1001); 5 Aug 2002 16:00:20 -0000
--* Received: from 4.star-private-mail-12.star.net.uk (HELO smtp-in-4.star.net.uk) (10.200.12.4)
--*   by delivery-2.star-private-mail-4.star.net.uk with SMTP; 5 Aug 2002 16:00:20 -0000
--* Received: (qmail 11234 invoked from network); 5 Aug 2002 16:00:19 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-4.star.net.uk with SMTP; 5 Aug 2002 16:00:19 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 9237 invoked from network); 5 Aug 2002 16:00:18 -0000
--* Received: from panoramix.inria.fr (138.96.111.9)
--*   by server-8.tower-17.messagelabs.com with SMTP; 5 Aug 2002 16:00:18 -0000
--* Received: (from bmanuel@localhost)
--* 	by panoramix.inria.fr (8.12.5/8.12.5) id g75G0HYl007343
--* 	for ax-bugs@nag.co.uk; Mon, 5 Aug 2002 18:00:17 +0200
--* Date: Mon, 5 Aug 2002 18:00:17 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200208051600.g75G0HYl007343@panoramix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] compile-time optimizer bug

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -q2 optbug.as
-- Version: 1.0.0
-- Original bug file name: optbug.as

--------------------------------- optbug.as ---------------------------------
--
-- This is a compile-time optimizer bug:
-- % aldor -q1 optbug.as
--
-- % aldor -q2 optbug.as
-- #1 (Fatal Error) Looking for `open' with code `58385643' in ``a`'.
-- Export of `(null)' not found.
--

#include "algebra"

macro Z == Integer;

Foo: with { two: () -> Z } == add {

	two():Z == {
		import from String, Maple, Partial Z, Partial ExpressionTree;

		session := maple();
		input(session) << "1+1:";
		result := run session;

		retract(eval(retract result)$Z);
	}
}


