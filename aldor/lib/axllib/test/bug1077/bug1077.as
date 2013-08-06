--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA07194; Thu, 2 May 96 23:03:36 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id SAA13090; Thu, 2 May 1996 18:00:03 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA32524; Thu, 2 May 1996 17:58:13 -0400
--* Date: Thu, 2 May 1996 17:58:13 -0400
--* Message-Id: <199605022158.AA32524@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [3] Another possible over-lazy domain initialization?

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Qall -Mno-mactext -Ginterp -DBUG1 -DBUG2 bug-domain-arg.as
-- Version: AXIOM-XL version 1.1.5 for AIX RS/6000
-- Original bug file name: bug-domain-arg.as

--+ cd ~/languages/asharp/bugs/
--+ axiomxl -V -Qall -Mno-mactext -Ginterp -DBUG1 -DBUG2 bug-domain-arg.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    2.9 s  0  2 .3  3  0  1  0  0  0 .3 84  2  6  2  0  0  0 .3 %
--+ 
--+  Source  204 lines,  4107 lines per minute
--+  Lib    5789 bytes,  1617syme 2295foam 56fsyme 583name 77kind 488file 282lazy 197type 10inl 14twins 2ext 2doc 19id
--+  Store  1912 K pool
--+ Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
--+ #2 (Warning) Removing file `bug-domain-arg.ao'.
--+ 
--+ Compilation exited abnormally with code 1 at Thu May  2 17:55:26
#include "axllib.as"
#pile

SI ==> SingleInteger

import from SI

f(n: SI):() ==

#if BUG1
  n = 42 => error "n is forty two"
#endif
#if BUG2
  FS == SingleIntegerMod n
#else
  FS ==> SingleIntegerMod n
#endif
  a: FS == sample
  print << a << newline

f(42)
