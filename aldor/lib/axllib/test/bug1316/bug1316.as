--* From Manuel.Bronstein@sophia.inria.fr  Wed Jun 27 16:24:57 2001
--* Received: from mail.london-1.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id QAA14445
--* 	for <ax-bugs@nag.co.uk>; Wed, 27 Jun 2001 16:24:56 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 22830 invoked from network); 27 Jun 2001 15:22:48 -0000
--* Received: from droopix.inria.fr (138.96.111.4)
--*   by server-7.tower-4.starlabs.net with SMTP; 27 Jun 2001 15:22:48 -0000
--* Received: by droopix.inria.fr (8.11.1/8.10.0) id f5RFOQR24704 for ax-bugs@nag.co.uk; Wed, 27 Jun 2001 17:24:26 +0200
--* Date: Wed, 27 Jun 2001 17:24:26 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200106271524.f5RFOQR24704@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] long constants corrupted in .ao file

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 -fao bad64ao.as followed by axiomxl -q2 -fc bad64ao.ao
-- Version: 1.1.13p1(5) for OSF/1 AXP
-- Original bug file name: bad64ao.as

---------------------------- bad64ao.as ------------------------------
--
-- This bug is reproducible only on a 64-bit DEC/OSF1
--
-- If the C file is produced directly, then everything's ok:
-- % axiomxl -q2 -fc bad64ao.as
--
-- On the other hand, going through the .ao file creates havoc:
-- % axiomxl -q2 -fao bad64ao.as
-- % axiomxl -q2 -fc bad64ao.ao
--

#include "axllib"

foo():SingleInteger == 427419669081;



