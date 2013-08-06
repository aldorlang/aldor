--* From mnd@knockdhu.cs.st-andrews.ac.uk  Mon Jul 12 11:15:13 1999
--* Received: from knockdhu.cs.st-andrews.ac.uk ([138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id LAA15112
--* 	for <ax-bugs@nag.co.uk>; Mon, 12 Jul 1999 11:14:55 +0100 (BST)
--* Received: (from mnd@localhost)
--* 	by knockdhu.cs.st-andrews.ac.uk (8.8.7/8.8.7) id LAA01968
--* 	for ax-bugs@nag.co.uk; Mon, 12 Jul 1999 11:15:13 +0100
--* Date: Mon, 12 Jul 1999 11:15:13 +0100
--* From: mnd <mnd@knockdhu.cs.st-andrews.ac.uk>
--* Message-Id: <199907121015.LAA01968@knockdhu.cs.st-andrews.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] Not inlining from parents

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Q4 -Wd+inline -Ffm mybool.as (ttry adding -DCompileTheWorkingVersion)
-- Version: 1.1.12p3
-- Original bug file name: /home/mnd/nag/compiler/aldor/1.1.12/testing/bugs/not-const/mybool.as

--+ The compiler is failing to inline exports from inherited domains when
--+ they are exported by both the parent and the child domains.
--+ 
--+ If the test program is compiled with -DCompileTheWorkingVersion then
--+ the /\, \/ and ~ exports from Boolean get inlined okay: the .fm file
--+ can be checked to show this. Compiling without the -D flag generates
--+ "Not const: Fail" messages for /\ and ~ showing that the optimiser
--+ can't inline them.
--+ 
--+ This problem only occurs if the export that we want to inline is also
--+ an export of our domain. In this example MyBoolean satisfies Logic
--+ and so it exports /\, \/ and ~. Since MyBoolean doesn't define these
--+ they are taken from the parent Boolean and for some reason they aren't
--+ given Const numbers (or their const numbers are changed to -1). The
--+ working version creates a version of MyBoolean which doesn't export
--+ these and they get inlined okay.

#include "axllib"

#if CompileTheWorkingVersion
-----------------------------------------------------------------
   MyBoolean : Join(Conditional, OrderedFinite) with
   {
      xor: (%, %) -> %;
   }
   == Boolean add
   {
      Rep == Boolean;
      import from Rep;

      xor(a:%, b:%):% ==
         per (((rep a) \/ (rep b)) /\ ~((rep a) /\ (rep b)));
   }
-----------------------------------------------------------------
#else
-----------------------------------------------------------------
   MyBoolean : Join(Conditional, Logic, OrderedFinite) with
   {
      xor: (%, %) -> %;
   }
   == Boolean add
   {
      xor(a:%, b:%):% == (a \/ b) /\ ~(a /\ b);
   }
-----------------------------------------------------------------
#endif

