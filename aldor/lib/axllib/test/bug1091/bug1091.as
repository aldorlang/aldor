--* From: Peter Broadbery <peterb>
--* Date: Tue, 13 Aug 96 10:18:57 BST
--* Received: from co.uk (nags8) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA15672; Tue, 13 Aug 96 10:18:59 BST
--* To: ax-bugs
--* Subject: [3] Two meanings for Enumeration

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl slist.as
-- Version: 1.1.[67]
-- Original bug file name: slist.as

#include "axllib"

r: 'rest' := rest;

SortedList(S:BasicType) : with {
   apply   : (%, 'rest') -> %;

   foo: % -> %;
} == List(S) add
{
   Rep ==> List(S);
   import from Rep;

   foo(x: %): % == x.rest;


}




