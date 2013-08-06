--* From chicha@scl.csd.uwo.ca  Sat Sep 29 15:39:12 2001
--* Received: from welly-2.star.net.uk (welly-2.star.net.uk [195.216.16.189])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA28517
--* 	for <ax-bugs@nag.co.uk>; Sat, 29 Sep 2001 15:39:11 +0100 (BST)
--* From: chicha@scl.csd.uwo.ca
--* Received: (qmail 8775 invoked by uid 1001); 29 Sep 2001 14:38:42 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by delivery-2.star-private-mail-4.star.net.uk with SMTP; 29 Sep 2001 14:38:42 -0000
--* Received: (qmail 29595 invoked from network); 29 Sep 2001 14:38:41 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 29 Sep 2001 14:38:41 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 8308 invoked from network); 29 Sep 2001 14:35:47 -0000
--* Received: from ptibonum.scl.csd.uwo.ca (129.100.16.102)
--*   by server-8.tower-17.messagelabs.com with SMTP; 29 Sep 2001 14:35:47 -0000
--* Message-Id: <200109291438.f8TEccP24091@plutonium.scl.csd.uwo.ca>
--* Date: Sat, 29 Sep 2001 10:38:38 -0400
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Test for the new bug server @aldor.org

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.0.0(7)
-- Original bug file name: /scl/people/chicha/titi.as

#include "aldor"
#include "aldorio"

import from DoubleFloat;
stdout << max$DoubleFloat << newline;
