--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA09377; Tue, 23 Apr 96 17:40:25 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id MAA18377; Tue, 23 Apr 1996 12:36:11 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA38804; Tue, 23 Apr 1996 12:33:55 -0400
--* Date: Tue, 23 Apr 1996 12:33:55 -0400
--* Message-Id: <199604231633.AA38804@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [3] Category defaults fetched too late during domain initialization.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -DBUG -Ginterp bug-default-timing.as
-- Version: AXIOM-XL version 1.1.5 for AIX RS/6000
-- Original bug file name: bug-default-timing.as

--+ axiomxl -V -Ginterp bug-default-timing.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    3.5 s  0  2 .3  3 .3  1 .3  0  0  1 86  4  3  1  0  0  0 .3 %
--+ 
--+  Source  221 lines,  3756 lines per minute
--+  Lib   10501 bytes,  2142syme 5271foam 120fsyme 704name 102kind 513file 346lazy 1119type 2inl 2twins 8ext 2doc 23id
--+  Store  2252 K pool
--+ 3
--+ axiomxl -V -DBUG -Ginterp bug-default-timing.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    3.7 s  0  1  1  4  0  1  0  1  0  1 85  3  3  1  0  0  0  1 %
--+ 
--+  Source  221 lines,  3517 lines per minute
--+  Lib   10545 bytes,  2142syme 5313foam 122fsyme 704name 102kind 513file 346lazy 1119type 2inl 2twins 8ext 2doc 23id
--+  Store  2320 K pool
--+ Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
--+ #2 (Warning) Removing file `bug-default-timing.ao'.
#include "axllib.as"
#pile

SI ==> SingleInteger
import from SI

foo(): Order with

    coerce: SI -> %

  == add

    Rep == SI
    import from Rep

    initializeDomain(): () ==
      x: % == coerce 1
      y: % == coerce 2
      b1: Boolean == x > y
#if BUG
      b2: Boolean == y < x
#endif

    coerce(i: SI): % == per i

    (a: %) = (b: %): Boolean == rep.a = rep.b

    (p: TextWriter) << (a: %): TextWriter == p << rep.a

    sample: % == per 42

    initializeDomain()

    (a: %) > (b: %): Boolean == rep.a > rep.b

F == foo()
f: F == coerce 3
print << f << newline
