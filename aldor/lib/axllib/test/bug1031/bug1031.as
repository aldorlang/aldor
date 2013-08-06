--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA14180; Fri, 29 Sep 95 16:15:21 BST
--* Received: from neptune.ethz.ch by sun2.nsfnet-relay.ac.uk with Internet SMTP 
--*           id <sg.19813-0@sun2.nsfnet-relay.ac.uk>;
--*           Fri, 29 Sep 1995 16:09:29 +0100
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) 
--*           by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id QAA13341 
--*           for <ax-bugs@nag.co.uk>; Fri, 29 Sep 1995 16:07:37 +0100
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id QAA06381 
--*           for ax-bugs@nag.co.uk; Fri, 29 Sep 1995 16:07:36 +0100
--* Date: Fri, 29 Sep 1995 16:07:36 +0100
--* Message-Id: <199509291507.QAA06381@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [3] DEC/OSF1 version has bad libfoam.a

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx hello.as
-- Version: 1.1.3 DEC/OSF1
-- Original bug file name: hello.as

------------------------- hello.as -------------------------------
--
-- Looks like libfoam.a is trashed in the DEC Alpha/OSF version
--
-- % axiomxl -Fx hello.as
-- ld:
-- Object file format error in: /home/nother/users/bronstei/axiomxl-1.1.3/lib/libfoam.a(store.o): unknown type of section
--
--
-- For information:
-- % uname -a
-- OSF1 nother V2.0 240 alpha
--

#include "axllib.as"

print << "Hello world" << newline


