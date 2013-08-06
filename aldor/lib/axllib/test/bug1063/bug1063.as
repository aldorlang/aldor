--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA08131; Fri, 12 Apr 96 14:59:23 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id JAA08096; Fri, 12 Apr 1996 09:54:50 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA19462; Fri, 12 Apr 1996 09:52:50 -0400
--* Date: Fri, 12 Apr 1996 09:52:50 -0400
--* Message-Id: <199604121352.AA19462@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [5] ^: (SI, SI) -> SI wrong for negative powers.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Ginterp bug-si-power.as
-- Version: AXIOM-XL version 1.1.5 for AIX RS/6000
-- Original bug file name: bug-si-power.as

--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    2.7 s  0 .4  1  2 .4 .4  0  0  0 .4 91  1  1  1  0  0  0  1 %
--+ 
--+  Source  196 lines,  4355 lines per minute
--+  Lib    6168 bytes,  1449syme 2960foam 78fsyme 547name 69kind 488file 234lazy 165type 2inl 8twins 2ext 2doc 17id
--+  Store  1776 K pool
--+ 123^-2 = 15129 ???
#include "axllib.as"
#pile

testSI(): () ==
  SI ==> SingleInteger
  import from SI

  i: SI == 123
  j: SI == -2

  print . "~a^~a = ~a ???~n" . (<< i, << j, << (i^j))

testSI()
