--* From chicha@scl.csd.uwo.ca  Sat Sep 29 15:25:24 2001
--* Received: from welly-3.star.net.uk (welly-3.star.net.uk [195.216.16.161])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA28340
--* 	for <ax-bugs@nag.co.uk>; Sat, 29 Sep 2001 15:25:23 +0100 (BST)
--* From: chicha@scl.csd.uwo.ca
--* Received: (qmail 12884 invoked from network); 29 Sep 2001 14:24:52 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by 203.star-private-mail-4.star.net.uk with SMTP; 29 Sep 2001 14:24:52 -0000
--* Received: (qmail 21594 invoked from network); 29 Sep 2001 14:24:52 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 29 Sep 2001 14:24:52 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 498 invoked from network); 29 Sep 2001 14:22:01 -0000
--* Received: from ptibonum.scl.csd.uwo.ca (129.100.16.102)
--*   by server-6.tower-17.messagelabs.com with SMTP; 29 Sep 2001 14:22:01 -0000
--* Message-Id: <200109291424.f8TEOii24041@plutonium.scl.csd.uwo.ca>
--* Date: Sat, 29 Sep 2001 10:24:44 -0400
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Test for the new bug server @aldor.org

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -fx -laldor titi.as
-- Version: 1.0.0(7)
-- Original bug file name: /scl/people/chicha/titi.as

#include "aldor"
#include "aldorio"

import from DoubleFloat;
stdout << max$DoubleFloat << newline;

