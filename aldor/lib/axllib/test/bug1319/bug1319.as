--* From bill@scl.csd.uwo.ca  Tue Jul 24 17:54:37 2001
--* Received: from mail.london-1.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id RAA27565
--* 	for <ax-bugs@nag.co.uk>; Tue, 24 Jul 2001 17:54:35 +0100 (BST)
--* From: bill@scl.csd.uwo.ca
--* X-VirusChecked: Checked
--* Received: (qmail 10790 invoked from network); 24 Jul 2001 16:50:57 -0000
--* Received: from ptibonum.scl.csd.uwo.ca (129.100.16.102)
--*   by server-7.tower-4.starlabs.net with SMTP; 24 Jul 2001 16:50:57 -0000
--* Message-Id: <200107241654.f6OGs1R02753@millennium.scl.csd.uwo.ca>
--* Date: Tue, 24 Jul 2001 12:54:01 -0400
--* To: ax-bugs@nag.co.uk
--* Subject: [1][compfault] coredumps on compilation

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fao -Fo -Fasy tst.as
-- Version: 1.1.12 for LINUX(glibc)
-- Original bug file name: /scl/people/bill/Aldor/MathML/tst.as

--+ file coredumps on compilation
--+ 
--+ axiomxl -Fao -Fo -Fasy tst.as
--+ Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
#include "axllib"

INT ==> Integer;

Tst:with == add {
  import from INT;
  (a,b) == (1,2)
}

