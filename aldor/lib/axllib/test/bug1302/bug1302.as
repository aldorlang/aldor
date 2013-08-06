--* From mnd@dcs.st-and.ac.uk  Wed Apr  4 11:15:42 2001
--* Received: from server-18.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id LAA28861
--* 	for <ax-bugs@nag.co.uk>; Wed, 4 Apr 2001 11:15:36 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 24714 invoked from network); 4 Apr 2001 10:12:55 -0000
--* Received: from host62-7-111-229.btinternet.com (HELO dcs.st-and.ac.uk) (62.7.111.229)
--*   by server-18.tower-4.starlabs.net with SMTP; 4 Apr 2001 10:12:55 -0000
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.11.0/8.11.0) id f34AH6R07105
--* 	for ax-bugs@nag.co.uk; Wed, 4 Apr 2001 11:17:06 +0100
--* Date: Wed, 4 Apr 2001 11:17:06 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200104041017.f34AH6R07105@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [2] Symes for type parameters confused

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source
-- Version: 1.1.13(21)
-- Original bug file name: bug1303.as


-- This bug demonstrates the compiler confusing domain parameters and
-- is intended to clarify the source of bug1302. Compile with:
--
--     axiomxl -DBROKEN bug1303.as
--
-- to get a strange type-checking error complaining that Node(UCH0) was
-- rejected because Node(UCH1) was expected. Replace Node(UCH0) with %
-- for a similar effect.

-- Bare-bones library.
Category:with == add;
Type:Category == with;
Tuple(T:Type):with == add;
(args:Tuple Type) -> (results:Tuple Type):with == add;


-- This compiles because there is no ambiguity between Blob, Node and %
Blob(UCH0:with):with
{
   null: Node(UCH0) -> NodeList(UCH0);
}
== add
{
   null(x:Node(UCH0)):NodeList(UCH0) == new(x)$NodeList(UCH0);
}


-- With -DBROKEN, this fails because of confusion between Node and %.
Node(UCH0:with): with
{
#if BROKEN
   null: Node(UCH0) -> NodeList(UCH0);
#endif
}
== add
{
#if BROKEN
   null(x:Node(UCH0)):NodeList(UCH0) == new(x)$NodeList(UCH0);
#endif
}


NodeList(UCH1:with): with
{
   new : Node(UCH1) -> %;
} == add
{
   new(x:Node(UCH1)):% == x pretend %;
}


