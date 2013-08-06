--* From youssef@mailer.scri.fsu.edu  Mon Feb  2 18:21:25 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA13411; Mon, 2 Feb 98 18:21:25 GMT
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id SAA23768 for <ax-bugs@nag.co.uk>; Mon, 2 Feb 1998 18:15:34 GMT
--* Received: from dirac (dirac.scri.fsu.edu [144.174.128.44]) by mailer.scri.fsu.edu (8.8.7/8.7.5) with SMTP id NAA23661; Mon, 2 Feb 1998 13:11:51 -0500 (EST)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by dirac (AIX 4.1/UCB 5.64) id AA174486; Mon, 2 Feb 1998 13:11:49 -0500
--* Date: Mon, 2 Feb 1998 13:11:49 -0500
--* Message-Id: <9802021811.AA174486@dirac>
--* To: adk@mailer.scri.fsu.edu, ax-bugs@nag.co.uk, edwards@mailer.scri.fsu.edu,
--*         youssef@mailer.scri.fsu.edu
--* Subject: [5] conditional export problem?

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp -Fx
-- Version: 1.1.10c
-- Original bug file name: t4.as

--+ --
--+ -- Peter,
--+ --
--+ --    Here is a kind of simple situation that sometimes comes up 
--+ -- where I'm not sure if its a limitation of Aldor or a bug.
--+ -- Although this is something that one might think "ought" to work,
--+ -- the compiler complains that << can't be re-defined.
--+ --
--+ --  Cheers,
--+ --     Saul Youssef
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ SI ==> SingleInteger
--+ 
--+ import from SI
--+ 
--+ FooDomain(T:Type): with {<<:(TextWriter,%)->TextWriter} == add
--+   Rep ==> T
--+   import from Rep,String
--+   
--+   if T has BasicType then
--+     <<(t:TextWriter,x:%):TextWriter == <<(t,rep x)
--+   else
--+     <<(t:TextWriter,x:%):TextWriter == <<(t,"UNPRINTABLE")
--+     
--
-- Peter,
--
--    Here is a kind of simple situation that sometimes comes up 
-- where I'm not sure if its a limitation of Aldor or a bug.
-- Although this is something that one might think "ought" to work,
-- the compiler complains that << can't be re-defined.
--
--  Cheers,
--     Saul Youssef
--
#include "axllib"
#pile

SI ==> SingleInteger

import from SI

FooDomain(T:Type): with {<<:(TextWriter,%)->TextWriter} == add
  Rep ==> T
  import from Rep,String
  
  if T has BasicType then
    <<(t:TextWriter,x:%):TextWriter == <<(t,rep x)
  else
    <<(t:TextWriter,x:%):TextWriter == <<(t,"UNPRINTABLE")
    
