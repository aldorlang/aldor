--* From mnd@dcs.st-and.ac.uk  Thu Nov  9 13:24:12 2000
--* Received: from server-12.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id NAA03393
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:24:10 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 25388 invoked from network); 9 Nov 2000 13:23:39 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-12.tower-4.starlabs.net with SMTP; 9 Nov 2000 13:23:39 -0000
--* Received: from dcs.st-and.ac.uk (knockdhu [138.251.206.239])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id NAA00563
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:23:19 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id NAA31264
--* 	for ax-bugs@nag.co.uk; Thu, 9 Nov 2000 13:24:33 GMT
--* Date: Thu, 9 Nov 2000 13:24:33 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011091324.NAA31264@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [5] pathnames lost for #library directives

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See the comments in the bug file
-- Version: 1.1.13(0)
-- Original bug file name: libbug.as


-- Store the following in extensions.as, compile to .ao and MOVE the .ao
-- into the directory above the current. Then start the interpreter and
-- execute the following commands:
-- #include "axllib"
-- #library LL "../extensions.ao"
-- import from LL
-- Character has with { dquote: % }
--

#include "axllib"

extend Character: with
{
   dquote: %;
}
== add
{
   dquote:% == char("_"");
}



