--* Date: Fri, 15 Sep 1995 17:52:26 -0400
--* From: asbugs@com.ibm.watson (S Watt)
--* Message-Id: <9509152152.AA20001@asharp.watson.ibm.com>
--* To: ax-bugs@uk.co.nag
--* Subject: forwbug
--* Sender: asbugs@com.ibm.watson

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


--  Reporter:    adk@scri.fsu.edu
--  Description: [1] A# program crashes if two variables have the same name.

------------------------------ bug-scope.as ---------------------------------
--* From postmaster@watson.vnet.ibm.com  Fri Sep 15 17:52:22 1995
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Date: Fri, 15 Sep 1995 17:52:20 -0400
--* Message-Id: <199509152152.AA26743@ibm12.scri.fsu.edu>
--* To: adk@mailer.scri.fsu.edu, asbugs@watson.ibm.com, infodesk@nag.co.uk
--* Subject: [1] A# program crashes if two variables have the same name.

-- Command line: axiomxl -V -Ginterp -DBUG bug-scope.as
-- Version: 1.1.3
-- Original bug file name: bug-scope.as

--+ axiomxl -V -Ginterp bug-scope.as
--+ AXIOM-XL version 1.1.3 for AIX RS/6000
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    4.6 s  0  2 .4  2 .2 .4 .2  0 .2  1 86  3  3  1  0  0  0 .2 %
--+
--+  Source  212 lines,  2759 lines per minute
--+  Lib    9956 bytes,  1617syme 6270foam 142fsyme 572name 77kind 488file 250lazy 365type 2inl 8twins 2ext 2doc 14id
--+  Store  2048 K pool
--+ sample@D = (((bar = 21))) <0>
--+ axiomxl -V -Ginterp -DBUG bug-scope.as
--+ AXIOM-XL version 1.1.3 for AIX RS/6000
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    4.4 s  0  1 .4  2 .2 .4 .2  0  0  1 87  3  2  2  0 .2  0 .4 %
--+
--+  Source  212 lines,  2839 lines per minute
--+  Lib    9973 bytes,  1617syme 6285foam 142fsyme 574name 77kind 488file 250lazy 365type 2inl 8twins 2ext 2doc 14id
--+  Store  2048 K pool
--+ sample@D = (((bar = Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
--+ #2 (Warning) Removing file `bug-scope.ao'.
#include "axllib"
#pile

#if BUG
Y ==> X
#endif

M(a: SingleInteger): BasicType == SingleInteger

G(X: BasicType, dep: X): BasicType == add

  local initializeDomain(Y: BasicType, bar: Y): SingleInteger ==
    print << "(((bar = " << bar << "))) " -- Dies here!
    42

  Rep == M initializeDomain(X, dep)
  import from Rep
  (a: %) = (b: %): Boolean == rep a = rep b
  (p: TextWriter) << (a: %): TextWriter == p << "<" << rep a << ">"
  sample: % == per sample
  hash(a: %): SingleInteger == hash rep a

test(): () ==
  import from SingleInteger
  D == G(SingleInteger, 21)
  import from D
  print << "sample@D = " << sample@D << newline

test()
