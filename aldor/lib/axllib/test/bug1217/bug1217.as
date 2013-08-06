--* From mnd@dcs.st-and.ac.uk  Mon May 22 15:32:44 2000
--* Received: from knockdhu.dcs.st-and.ac.uk (knockdhu.dcs.st-and.ac.uk [138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id PAA06996
--* 	for <ax-bugs@nag.co.uk>; Mon, 22 May 2000 15:32:39 +0100 (BST)
--* Received: (from mnd@dcs.st-and.ac.uk)
--* 	by knockdhu.dcs.st-and.ac.uk (8.8.7/8.8.7) id PAA20120
--* 	for ax-bugs@nag.co.uk; Mon, 22 May 2000 15:38:35 +0100
--* Date: Mon, 22 May 2000 15:38:35 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200005221438.PAA20120@knockdhu.dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [5] segfault when compiling domain with missing export

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DSEGFAULT -grun -laxllib en01.as
-- Version: 1.1.12p5
-- Original bug file name: en01.as


#include "axllib"

Colour == 'red,green,blue';

FooDom:with
{
   new: Colour -> %;
   apply: (%, 'value') -> Colour;
#if SEGFAULT
   set: (%, 'value', Colour) -> Colour;
#else
   set!: (%, 'value', Colour) -> Colour;
#endif
   export from 'value';
}
== add
{
   Rep == Record(val:Colour);
   import from Rep, 'value';

   new(x:Colour):% == per [x];
   apply(x:%, ignored:'value'):Colour == (rep x).val;
   set!(x:%, ignored:'value', v:Colour):Colour == { (rep x).val := v }
}


main():() ==
{
   import from SingleInteger, Colour;

   local var:FooDom;

   var := new red;
   print << "var = " << ((var.value) pretend SingleInteger) << newline;

   var.value := green;
   print << "var = " << ((var.value) pretend SingleInteger) << newline;

   var.value := blue;
   print << "var = " << ((var.value) pretend SingleInteger) << newline;
}


main();
