--* From mnd@dCS.st-and.ac.uk  Tue Oct 19 15:39:33 1999
--* Received: from pittyvaich.dcs.st-and.ac.uk (pittyvaich.dcs.st-and.ac.uk [138.251.206.55])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id PAA23060
--* 	for <ax-bugs@nag.co.uk>; Tue, 19 Oct 1999 15:39:18 +0100 (BST)
--* Received: from kininvie.dcs.st-and.ac.uk (kininvie [138.251.206.236])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id PAA19042
--* 	for <ax-bugs@nag.co.uk>; Tue, 19 Oct 1999 15:39:07 +0100 (BST)
--* Received: (from mnd@localhost) by kininvie.dcs.st-and.ac.uk (8.9.3/8.6.12) id PAA04276 for ax-bugs@nag.co.uk; Tue, 19 Oct 1999 15:38:23 +0100
--* Date: Tue, 19 Oct 1999 15:38:23 +0100
--* From: "Martin N. Dunstan" <mnd@dCS.st-and.ac.uk>
--* Message-Id: <199910191438.PAA04276@kininvie.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9][other=axllib] Nasty things with constant strings

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Q0 -Fx bug01.as
-- Version: 1.1.12p4 (personal edition)
-- Original bug file name: bug01.as


#include "axllib"

import from String;

-- Use the debugging version of libfoam (auditing enabled)
dispose!("This is  a test");
