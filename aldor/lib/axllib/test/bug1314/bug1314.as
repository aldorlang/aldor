--* From mnd@dcs.st-and.ac.uk  Sat Jun  9 12:22:55 2001
--* Received: from server-12.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id MAA26192
--* 	for <ax-bugs@nag.co.uk>; Sat, 9 Jun 2001 12:22:54 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 11414 invoked from network); 9 Jun 2001 11:13:58 -0000
--* Received: from du-019-0045.claranet.co.uk (HELO dcs.st-and.ac.uk) (195.8.76.45)
--*   by server-12.tower-4.starlabs.net with SMTP; 9 Jun 2001 11:13:58 -0000
--* Received: (from mnd@localhost)
--* 	by dcs.st-and.ac.uk (8.11.0/8.11.0) id f59BIOK13769
--* 	for ax-bugs@nag.co.uk; Sat, 9 Jun 2001 12:18:24 +0100
--* Date: Sat, 9 Jun 2001 12:18:24 +0100
--* From: mnd <mnd@dcs.st-and.ac.uk>
--* Message-Id: <200106091118.f59BIOK13769@dcs.st-and.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [1][other=foam_c] fiArrNew_Ptr not defined

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib arrbug1.as
-- Version: 1.1.13(27)
-- Original bug file name: arrbug1.as


-- Try "axiomxl -grun -laxllib arrbug1.as" and note the link error:
--
-- arrbug1.o: In function `CF4_PackedArrayNew':
-- arrbug1.o(.text+0x9b3): undefined reference to `fiArrNew_Ptr'
-- collect2: ld returned 1 exit status
-- #1 (Fatal Error) Linker failed.  Command was: unicl arrbug1.o axlmain.o
--     -L. -L/home/mnd/nag/compiler/aldor/1.1.13/base/linux/share/lib
--     -L/home/mnd/nag/compiler/aldor/1.1.13/base/linux/lib -o  arrbug1
--     -laxllib -lfoam
-- #1 (Warning) Removing file `arrbug1.o'.
-- #2 (Warning) Removing file `axlmain.o'.
--
-- The fix is to add fiArrNew_Ptr to foam_c.c.

#include "axllib"

PtrDom:DenseStorageCategory with
{
   new: SingleInteger -> %;
}
== add
{
   import from Machine;
   Rep == Ptr;
   import from Rep;
   new(n:SingleInteger):% == per nil;
}

