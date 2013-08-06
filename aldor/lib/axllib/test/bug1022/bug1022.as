--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA18928; Sun, 17 Sep 95 06:56:55 BST
--* Received: from mailer.scri.fsu.edu by sun3.nsfnet-relay.ac.uk 
--*           with Internet SMTP id <g.02848-0@sun3.nsfnet-relay.ac.uk>;
--*           Sun, 17 Sep 1995 06:53:11 +0100
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) 
--*           by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id BAA05497;
--*           Sun, 17 Sep 1995 01:53:27 -0400
--* From: Tony Kennedy <adk@edu.fsu.scri>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA38926;
--*           Sun, 17 Sep 1995 01:52:59 -0400
--* Date: Sun, 17 Sep 1995 01:52:59 -0400
--* Message-Id: <199509170552.AA38926@ibm4.scri.fsu.edu>
--* To: adk@edu.fsu.scri, ax-bugs@uk.co.nag, edwards@edu.fsu.scri
--* Subject: [1] Bad arguments being passed to domain constructor (65 != 4)
--* Sender: adk@edu.fsu.scri.mailer

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Ginterp bug-lazy.as
-- Version: 1.1.3 for AIX RS/6000
-- Original bug file name: bug-lazy.as

--+ axiomxl -V -Ginterp bug-lazy.as
--+ AXIOM-XL version 1.1.3 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    2.9 s  0  2 .3  2 .3  1 .3  0  0  1 86  3  4  1  0  0  0  0 %
--+ 
--+  Source  212 lines,  4341 lines per minute
--+  Lib   11081 bytes,  1575syme 7528foam 144fsyme 572name 75kind 488file 250lazy 275type 2inl 8twins 2ext 2doc 13id
--+  Store  1980 K pool
--+ Starting test...
--+ Constructing E(23)
--+ Rep == D(65)
--+ Constructing D(4)
--+ sample@F = E<0>
--+ ...ending test
#include "axllib.as"
#pile

SI ==> SingleInteger

import from SI

D(darg: SI): BasicType == SI add
  print."Constructing D(~1)~n" (<< darg)

E(earg: SI): BasicType == add
  print."Constructing E(~1)~n" (<< earg)
  local t: SI
  t := earg + 42
  print."Rep == D(~1)~n" (<< t)
  Rep == D(t)
  import from Rep
  (a:%) = (b:%): Boolean == (rep a) = (rep b)
  sample: % == per sample
  (p: TextWriter) << (a: %): TextWriter == print."E<~1>" (<< rep a)

test(): () ==
  print."Starting test...~n"
  F == E(23)
  import from F
  print."sample@F = ~1~n" (<< sample@F)
  print."...ending test~n"

test()
