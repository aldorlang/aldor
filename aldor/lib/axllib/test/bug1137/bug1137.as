--* From youssef@mailer.scri.fsu.edu  Fri Jan 23 02:51:02 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA12794; Fri, 23 Jan 98 02:51:02 GMT
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id CAA02032 for <ax-bugs@nag.co.uk>; Fri, 23 Jan 1998 02:53:20 GMT
--* Received: from sp2-8.scri.fsu.edu (sp2-8.scri.fsu.edu [144.174.128.98]) by mailer.scri.fsu.edu (8.8.7/8.7.5) with SMTP id VAA03921; Thu, 22 Jan 1998 21:50:46 -0500 (EST)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by sp2-8.scri.fsu.edu (5.67b) id AA21279; Thu, 22 Jan 1998 21:50:45 -0500
--* Date: Thu, 22 Jan 1998 21:50:45 -0500
--* Message-Id: <199801230250.AA21279@sp2-8.scri.fsu.edu>
--* To: adk@mailer.scri.fsu.edu, ax-bugs@nag.co.uk, edwards@mailer.scri.fsu.edu,
--*         youssef@mailer.scri.fsu.edu
--* Subject: [5] Union funny business

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp -Fx file.as
-- Version: 1.1.10c
-- Original bug file name: bug0.as

--+ --
--+ -- Peter:
--+ --
--+ --    I don't think that the results I get with this program are
--+ --  correct.  I would have expected the compiler to complain that
--+ --  = is not defined for unions, but it seems to produce results
--+ --  anyway.
--+ --
--+ --     Regards,
--+ --       Saul Youssef
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ SI ==> SingleInteger
--+ 
--+ import from SI
--+ 
--+ U == Union(foointeger:SI, barinteger:SI)
--+ 
--+ foo42:U := [foointeger == 42]
--+ bar42:U := [barinteger == 42]
--+ foo42also:U := [foointeger == 42]
--+ 
--+ if foo42=bar42 then print << "foo42=bar42" << newline
--+ else
--+    print << "foo42 ~= bar42" << newline
--+  
--+ if foo42=foo42also then print << "foo42=foo42also" << newline
--+ else
--+    print << "foo42 ~=foo42_also" << newline
--
-- Peter:
--
--    I don't think that the results I get with this program are
--  correct.  I would have expected the compiler to complain that
--  = is not defined for unions, but it seems to produce results
--  anyway.
--
--     Regards,
--       Saul Youssef
--
#include "axllib"
#pile

SI ==> SingleInteger

import from SI

U == Union(foointeger:SI, barinteger:SI)

foo42:U := [foointeger == 42]
bar42:U := [barinteger == 42]
foo42also:U := [foointeger == 42]

if foo42=bar42 then print << "foo42=bar42" << newline
else
   print << "foo42 ~= bar42" << newline
 
if foo42=foo42also then print << "foo42=foo42also" << newline
else
   print << "foo42 ~=foo42_also" << newline
