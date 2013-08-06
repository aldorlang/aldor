--* From mnd@dcs.st-and.ac.uk  Tue Oct 17 01:21:31 2000
--* Received: from server-6.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id BAA09112
--* 	for <ax-bugs@nag.co.uk>; Tue, 17 Oct 2000 01:21:24 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 22386 invoked from network); 17 Oct 2000 00:20:49 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-6.tower-4.starlabs.net with SMTP; 17 Oct 2000 00:20:49 -0000
--* Received: from dcs.st-and.ac.uk (ara3263-ppp [138.251.206.30])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id NAA22852
--* 	for <ax-bugs@nag.co.uk>; Mon, 16 Oct 2000 13:16:31 +0100 (BST)
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id NAA25970
--* 	for ax-bugs@nag.co.uk; Mon, 16 Oct 2000 13:18:43 +0100
--* Date: Mon, 16 Oct 2000 13:18:43 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200010161218.NAA25970@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [5][interp] Cannot access cmdline args from interpreter

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -ginterp cmdlinebug.as
-- Version: 1.1.13(0)
-- Original bug file name: cmdlinebug.as


#include "axllib"

import from CommandLine, Array(String);

-- Segfaults with -ginterp
local argc:SingleInteger := #arguments;

print << "You gave " << argc << " arguments." << newline;

