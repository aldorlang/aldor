--* From youssef@d0mino.fnal.gov  Mon Nov  6 18:09:59 2000
--* Received: from server-3.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id SAA08740
--* 	for <ax-bugs@nag.co.uk>; Mon, 6 Nov 2000 18:09:52 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 19019 invoked from network); 6 Nov 2000 18:01:37 -0000
--* Received: from d0mino.fnal.gov (131.225.224.45)
--*   by server-3.tower-4.starlabs.net with SMTP; 6 Nov 2000 18:01:37 -0000
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id MAA81398;
--* 	Mon, 6 Nov 2000 12:09:15 -0600 (CST)
--* Date: Mon, 6 Nov 2000 12:09:15 -0600 (CST)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200011061809.MAA81398@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: domain.as

--+ --
--+ --  Hi Martin,
--+ --
--+ --     This core dumps.  Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ Foo:(SingleInteger)->with == (i:SingleInteger):with +-> Integer
--
--  Hi Martin,
--
--     This core dumps.  Saul
--
#include "axllib"
#pile

Foo:(SingleInteger)->with == (i:SingleInteger):with +-> Integer
