--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA20301; Sun, 17 Sep 95 23:25:05 BST
--* Received: from mailer.scri.fsu.edu by sun3.nsfnet-relay.ac.uk 
--*           with Internet SMTP id <sg.22593-0@sun3.nsfnet-relay.ac.uk>;
--*           Sun, 17 Sep 1995 23:20:46 +0100
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) 
--*           by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id SAA08486;
--*           Sun, 17 Sep 1995 18:21:02 -0400
--* From: Tony Kennedy <adk@edu.fsu.scri>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA35878;
--*           Sun, 17 Sep 1995 18:20:41 -0400
--* Date: Sun, 17 Sep 1995 18:20:41 -0400
--* Message-Id: <199509172220.AA35878@ibm4.scri.fsu.edu>
--* To: adk@edu.fsu.scri, ax-bugs@uk.co.nag, edwards@edu.fsu.scri
--* Subject: [1] Segmentation violation in domain construction.
--* Sender: adk@edu.fsu.scri.mailer

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Ginterp bug-lazy-2.as
-- Version: 1.1.3
-- Original bug file name: bug-lazy-2.as

--+ axiomxl -V -Ginterp bug-lazy-2.as
--+ AXIOM-XL version 1.1.3 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    3.4 s  0 .3  1  1 .3 .3  0 .3  0  1 90  3  3  1  0  0  0  0 %
--+ 
--+  Source  207 lines,  3620 lines per minute
--+  Lib   11370 bytes,  2142syme 6539foam 144fsyme 690name 102kind 513file 418lazy 646type 2inl 8twins 2ext 2doc 15id
--+  Store  2592 K pool
--+ Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
--+ #2 (Warning) Removing file `bug-lazy-2.ao'.
#include "axllib.as"
#pile

SI ==> SingleInteger

import from SI

D(darg: SI): BasicType == SI

E(X: BasicType, earg: List X): BasicType == add
  local t(targ: List X): SI == #targ
  Rep == D(t(earg))
  import from Rep
  (a:%) = (b:%): Boolean == (rep a) = (rep b)
  sample: % == per sample
  (p: TextWriter) << (a: %): TextWriter == print."E<~1>" (<< rep a)

test(): () ==
  import from List String
  F == E(String, [])
  import from F
  print."sample@F = ~1~n" (<< sample@F)

test()
