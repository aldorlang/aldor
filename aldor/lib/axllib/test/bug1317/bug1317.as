--* From mnd@dcs.st-and.ac.uk  Tue Jul 17 12:01:27 2001
--* Received: from mail.london-1.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id MAA10104
--* 	for <ax-bugs@nag.co.uk>; Tue, 17 Jul 2001 12:01:26 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 27003 invoked from network); 17 Jul 2001 10:58:08 -0000
--* Received: from host213-122-12-221.btinternet.com (HELO dcs.st-and.ac.uk) (213.122.12.221)
--*   by server-7.tower-4.starlabs.net with SMTP; 17 Jul 2001 10:58:08 -0000
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.11.0/8.11.0) id f6HAgSe12789
--* 	for ax-bugs@nag.co.uk; Tue, 17 Jul 2001 11:42:28 +0100
--* Date: Tue, 17 Jul 2001 11:42:28 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200107171042.f6HAgSe12789@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [1][other=foam_c] Missing fiArrNew_ functions

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: See source comments
-- Version: 1.1.13(30)
-- Original bug file name: bug1317.as


#include "axllib"

-- axiomxl -grun -laxllib bug1317.as fails because fiArrNew_BInt isn't
-- defined. Similar bugs can be forced using closures for Rep etc. I've
-- just fixed this.

BIntArray:with
{
   new: SingleInteger -> BArr;
}
== add
{
   Rep == BBInt;
   import from Rep, Integer;

   import
   {
      ArrNew: (BBInt, BSInt) -> BArr;
      ArrElt: (BArr,  BSInt) -> Rep;
      ArrSet: (BArr,  BSInt, Rep) -> Rep;
   } from Builtin;

   new(n:SingleInteger):BArr == ArrNew(0::BBInt, n::BSInt)
}


main():() ==
{
   import from SingleInteger, BIntArray;
   local x:BArr := new(42);
}


main();

