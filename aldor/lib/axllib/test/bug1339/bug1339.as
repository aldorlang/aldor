--* From Manuel.Bronstein@sophia.inria.fr  Tue Jul  2 14:18:04 2002
--* Received: from welly-1.star.net.uk (welly-1.star.net.uk [195.216.16.165])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id OAA16602
--* 	for <ax-bugs@nag.co.uk>; Tue, 2 Jul 2002 14:18:01 +0100 (BST)
--* Received: (qmail 6170 invoked from network); 2 Jul 2002 13:17:31 -0000
--* Received: from 6.star-private-mail-12.star.net.uk (HELO smtp-in-6.star.net.uk) (10.200.12.6)
--*   by delivery-1.star-private-mail-4.star.net.uk with SMTP; 2 Jul 2002 13:17:31 -0000
--* Received: (qmail 21431 invoked from network); 2 Jul 2002 13:17:30 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-6.star.net.uk with SMTP; 2 Jul 2002 13:17:30 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 28203 invoked from network); 2 Jul 2002 13:17:30 -0000
--* Received: from automatix.inria.fr (138.96.111.13)
--*   by server-9.tower-17.messagelabs.com with SMTP; 2 Jul 2002 13:17:30 -0000
--* Received: by automatix.inria.fr (8.11.6/8.11.6) id g62DHTg10067 for ax-bugs@nag.co.uk; Tue, 2 Jul 2002 15:17:29 +0200
--* Date: Tue, 2 Jul 2002 15:17:29 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200207021317.g62DHTg10067@automatix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [5] bad '==' assignment

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -ginterp badhas.as
-- Version: 1.0.1
-- Original bug file name: badhas.as

---------------------------- badhas.as -----------------------------
--
-- THIS IS THE ROOT-CAUSE OF MARC's PROBLEMS WITH SUP/RMP/SMP
--
-- % aldor -ginterp badhas.as
-- b0 = false
-- has? = true
-- false
--
-- This bug disappears if 'b0:Boolean ==' becomes 'local b0:Boolean =='
--

#include "axllib"

Foo(R:Ring):Ring with { foo?: () -> Boolean } == R add {
	local lfoo?:Boolean == {
		b0: Boolean == (R has EuclideanDomain);
		print << "b0 = " << b0 << newline;
		print << "has? = " << (R has EuclideanDomain) << newline;
		b0;
	}

	foo?():Boolean == lfoo?;
}

print << foo?()$Foo(Integer) << newline;


