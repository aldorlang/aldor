--* From mnd@dcs.st-and.ac.uk  Thu Nov  9 13:32:02 2000
--* Received: from server-3.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id NAA03507
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:32:02 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 29061 invoked from network); 9 Nov 2000 13:23:37 -0000
--* Received: from pittyvaich.dcs.st-and.ac.uk (138.251.206.55)
--*   by server-3.tower-4.starlabs.net with SMTP; 9 Nov 2000 13:23:37 -0000
--* Received: from dcs.st-and.ac.uk (knockdhu [138.251.206.239])
--* 	by pittyvaich.dcs.st-and.ac.uk (8.9.1b+Sun/8.9.1) with ESMTP id NAA00922
--* 	for <ax-bugs@nag.co.uk>; Thu, 9 Nov 2000 13:31:10 GMT
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.8.7/8.8.7) id NAA31283
--* 	for ax-bugs@nag.co.uk; Thu, 9 Nov 2000 13:32:25 GMT
--* Date: Thu, 9 Nov 2000 13:32:25 GMT
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200011091332.NAA31283@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [6] multiple extensions do not work

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source code comments
-- Version: 1.1.13(0)
-- Original bug file name: extenbug.as


#include "axllib"

-- Compile this and then try using +$String in a program - the export
-- does not appear to be visible at all. It is not the result of edit
-- 12p6(6) or 12p6(25) because disabling either does not fix the bug.

-- Since String has already been extended once, perhaps this is a bug
-- involving multiple linear extensions: multiple diamond extensions
-- are already known to fail but this ...?

-- However, under the interpreter we get to see all the operations
-- including these extensions. Also we can perform "has" tests and
-- find that the operations are indeed there:
--
--    #include "axllib"
--    #library LL "extenbug.ao"
--    import from LL
--    import from String
--    +
--    String has with { + : (%, %) -> % }


extend String:with
{
   * : (%, %) -> %;
   + : (String, String) -> String;
}
== add
{
   -- Concatentation of strings using infix + is very useful.
   (a:String) + (b:String):String == concat(a, b);


   -- Same idea but using % rather than String.
   (a:%) * (b:%):% == concat(a, b);
}




