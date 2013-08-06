--* From mnd@knockdhu.dcs.st-and.ac.uk  Tue Mar  7 12:35:18 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id MAA23472
--* 	for <ax-bugs@nag.co.uk>; Tue, 7 Mar 2000 12:35:13 GMT
--* Received: (from mnd@localhost)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id MAA18795
--* 	for ax-bugs@nag.co.uk; Tue, 7 Mar 2000 12:40:52 GMT
--* Date: Tue, 7 Mar 2000 12:40:52 GMT
--* From: mnd <mnd@knockdhu.dcs.st-and.ac.uk>
--* Message-Id: <200003071240.MAA18795@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] Subtle parsing of `with' expressions

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source
-- Version: 1.1.12p5
-- Original bug file name: with00.as


-- Compile with -Ffm -Wtr+scan+linear+parse. Note the introduction of
-- a semicolon after the closing } of the MyCat definition. Although
-- this interpretation is perfectly legal, it is unfortunate that the
-- definition of MyCat silently loses the `with OtherCat' part.

Category:with == add;
define BaseCat == with;
define OtherCat == with;


-- The following line is "incorrectly" parsed as:
--    define MyCat:Category == with BaseCat; with OtherCat;
-- (note the placing of the semicolons in the code above).
define MyCat:Category == { with BaseCat } with OtherCat;

