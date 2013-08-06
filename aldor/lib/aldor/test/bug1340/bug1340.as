--* From Manuel.Bronstein@sophia.inria.fr  Tue Jul 16 18:30:29 2002
--* Received: from welly-1.star.net.uk (welly-1.star.net.uk [195.216.16.165])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id SAA29002
--* 	for <ax-bugs@nag.co.uk>; Tue, 16 Jul 2002 18:30:28 +0100 (BST)
--* Received: (qmail 14501 invoked from network); 16 Jul 2002 17:30:00 -0000
--* Received: from 4.star-private-mail-12.star.net.uk (HELO smtp-in-4.star.net.uk) (10.200.12.4)
--*   by delivery-1.star-private-mail-4.star.net.uk with SMTP; 16 Jul 2002 17:30:00 -0000
--* Received: (qmail 16908 invoked from network); 16 Jul 2002 17:29:59 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-4.star.net.uk with SMTP; 16 Jul 2002 17:29:59 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 2267 invoked from network); 16 Jul 2002 17:29:58 -0000
--* Received: from panoramix.inria.fr (138.96.111.9)
--*   by server-5.tower-17.messagelabs.com with SMTP; 16 Jul 2002 17:29:58 -0000
--* Received: by panoramix.inria.fr (8.11.6/8.11.6) id g6GHTwA11111 for ax-bugs@nag.co.uk; Tue, 16 Jul 2002 19:29:58 +0200
--* Date: Tue, 16 Jul 2002 19:29:58 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200207161729.g6GHTwA11111@panoramix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] yet another -q2 --> runtime seg fault

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fx -laldor dblout.as
-- Version: 1.0.0
-- Original bug file name: dblout.as

----------------------------- dblout.as ------------------------
--
-- This illustrates a serious optimizer bug in << from DoubleFloat:
--
-- % aldor -fx -laldor dblout.as
-- % dblout  --> Segmentation fault
--
-- % aldor -fo -q1 sal_dfloat.as
-- % aldor -fx -laldor dblout.as sal_dfloat.o
-- % dblout  --> works
--

#include "aldor"
#include "aldorio"

import from DoubleFloat;

main():()=={
	stdout << 1.0 << newline;
}
main();
