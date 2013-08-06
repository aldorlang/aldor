--* From chicha@scl.csd.uwo.ca  Sat Sep 29 15:32:00 2001
--* Received: from welly-2.star.net.uk (welly-2.star.net.uk [195.216.16.189])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA28427
--* 	for <ax-bugs@nag.co.uk>; Sat, 29 Sep 2001 15:31:59 +0100 (BST)
--* From: chicha@scl.csd.uwo.ca
--* Received: (qmail 8274 invoked by uid 1001); 29 Sep 2001 14:31:30 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by delivery-2.star-private-mail-4.star.net.uk with SMTP; 29 Sep 2001 14:31:30 -0000
--* Received: (qmail 25517 invoked from network); 29 Sep 2001 14:31:29 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 29 Sep 2001 14:31:29 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 9642 invoked from network); 29 Sep 2001 14:28:33 -0000
--* Received: from ptibonum.scl.csd.uwo.ca (129.100.16.102)
--*   by server-14.tower-17.messagelabs.com with SMTP; 29 Sep 2001 14:28:33 -0000
--* Message-Id: <200109291431.f8TEVR524059@plutonium.scl.csd.uwo.ca>
--* Date: Sat, 29 Sep 2001 10:31:27 -0400
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

