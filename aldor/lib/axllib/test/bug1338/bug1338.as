--* From Manuel.Bronstein@sophia.inria.fr  Fri Jun 14 12:13:59 2002
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id MAA05891
--* 	for <ax-bugs@nag.co.uk>; Fri, 14 Jun 2002 12:13:58 +0100 (BST)
--* Received: (qmail 21437 invoked from network); 14 Jun 2002 11:13:29 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 14 Jun 2002 11:13:29 -0000
--* Received: (qmail 7853 invoked from network); 14 Jun 2002 11:13:29 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 14 Jun 2002 11:13:29 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 15404 invoked from network); 14 Jun 2002 11:13:28 -0000
--* Received: from automatix.inria.fr (138.96.111.13)
--*   by server-7.tower-17.messagelabs.com with SMTP; 14 Jun 2002 11:13:28 -0000
--* Received: by automatix.inria.fr (8.11.6/8.11.6) id g5EBAmr03814 for ax-bugs@nag.co.uk; Fri, 14 Jun 2002 13:10:48 +0200
--* Date: Fri, 14 Jun 2002 13:10:48 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200206141110.g5EBAmr03814@automatix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [4] BIntIsOdd$Builtin suboptimal

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -q5 -fc oddBInt.as
-- Version: 1.0.0
-- Original bug file name: oddBInt.as

---------------------------- oddBInt.as -------------------------
--
-- The function BIntIsOdd in the runtime is poorly implemented:
-- it does an allocation for the BInt 2 and a full euclidean division
-- as shown in the C code for foo below.
-- Instead, it should test the lsb without any allocation
-- as shown in the C code for bar.
-- GMPInteger from libaldor does the right thing,
-- but AldorInteger reroutes odd? to BIntIsOdd.
--

#include "axllib"

import from Machine;

foo?(n:BInt):Bool == odd? n;

-- static FiBool
-- CF1_foo_QMARK_(FiEnv e1, FiBInt P0_n)
-- {
--         return fiBIntNE(fiBIntMod(P0_n, fiBIntNew(2L)), fiBInt0());
-- }


bar?(n:BInt):Bool == { import from SInt; bit(n, 0); }

-- static FiBool
-- CF2_bar_QMARK_(FiEnv e1, FiBInt P0_n)
-- {
--         FiBool T0;
--         fiBINT_BIT(T0, FiBool, P0_n, 0L);
--         return T0;
-- }



