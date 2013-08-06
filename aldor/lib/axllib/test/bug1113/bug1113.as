--* From gautier@inf.ethz.ch  Fri Feb 28 15:20:03 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA24436; Fri, 28 Feb 97 15:20:03 GMT
--* Received: from inf.ethz.ch (root@neptune.ethz.ch [129.132.10.10])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with SMTP
--* 	  id PAA08579 for <ax-bugs@nag.co.uk>; Fri, 28 Feb 1997 15:21:00 GMT
--* Received: from vinci.inf.ethz.ch (gautier@vinci.inf.ethz.ch [129.132.12.46]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id PAA13300 for <ax-bugs@nag.co.uk>; Fri, 28 Feb 1997 15:56:32 +0100
--* From: Thierry Gautier <gautier@inf.ethz.ch>
--* Received: (gautier@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id PAA05403 for ax-bugs@nag.co.uk; Fri, 28 Feb 1997 15:56:30 +0100
--* Date: Fri, 28 Feb 1997 15:56:30 +0100
--* Message-Id: <199702281456.PAA05403@vinci.inf.ethz.ch>
--* To: ax-bugs@nag.co.uk
--* Subject: [6] seg fault in fi- function

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.9
-- Original bug file name: formatbug.as

--------------------------- formatbug.as ---------------------
--
-- On both sun versions (sunos and solaris) causes a seg fault
-- inside fiFormatSInt (followed by memmove).
-- works on hpux and axposf1
--
-- < gautier@mendel:402 > axiomxl -fx formatbug.as
-- < gautier@mendel:403 > formatbug
-- Segmentation fault (core dumped)

#include "axllib"

import from SingleInteger ;
import from Format ;


str: String := "       " ;
format( 1, str, 1) ;
print << str << newline ;

