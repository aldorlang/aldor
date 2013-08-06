--* From mnd@dcs.st-and.ac.uk  Wed Apr  4 11:15:51 2001
--* Received: from server-18.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id LAA28892
--* 	for <ax-bugs@nag.co.uk>; Wed, 4 Apr 2001 11:15:41 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 24730 invoked from network); 4 Apr 2001 10:12:56 -0000
--* Received: from host62-7-111-229.btinternet.com (HELO dcs.st-and.ac.uk) (62.7.111.229)
--*   by server-18.tower-4.starlabs.net with SMTP; 4 Apr 2001 10:12:56 -0000
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.11.0/8.11.0) id f34AGS407091
--* 	for ax-bugs@nag.co.uk; Wed, 4 Apr 2001 11:16:28 +0100
--* Date: Wed, 4 Apr 2001 11:16:28 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200104041016.f34AGS407091@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] Symes for type parameters confused

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source
-- Version: 1.1.13(21)
-- Original bug file name: bug1302.as


-- This bug demonstrates the compiler confusing domain parameters. With
-- -DBROKEN this file will compile but the syme for UCH in null$Node()
-- is the one from NodeList() not Node(). Compile with:
--
--     axiomxl -DBROKEN -Wd+genfoamHash bug1302.as
--
-- and get an "ugh" warning (you'll need to fix genfHashDEBUG first ;)
--
-- For the source of this problem, see bug1303. This file is a trimmed
-- version of a bug reported by Bill Naylor.


-- Bare-bones library.
Category:with == add;
Type:Category == with;
Tuple(T:Type):with == add;
(args:Tuple Type) -> (results:Tuple Type):with == add;


-- This compiles because there is no ambiguity between Blob, Node and %
Blob(UCH:with):with
{
   null: Node(UCH) -> NodeList(UCH);
}
== add
{
   null(x:Node(UCH)):NodeList(UCH) == new(x)$NodeList(UCH);
}


-- With -DBROKEN, this fails because of confusion between Node and %.
Node(UCH:with): with
{
#if BROKEN
   null: Node(UCH) -> NodeList(UCH);
#endif
}
== add
{
#if BROKEN
   null(x:Node(UCH)):NodeList(UCH) == new(x)$NodeList(UCH);
#endif
}


NodeList(UCH:with): with
{
   new : Node(UCH) -> %;
} == add
{
   new(x:Node(UCH)):% == x pretend %;
}


