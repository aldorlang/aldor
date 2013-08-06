--* From youssef@mailer.scri.fsu.edu  Tue Aug 26 18:59:41 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA16124; Tue, 26 Aug 97 18:59:41 +0100
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id TAA22502 for <ax-bugs@nag.co.uk>; Tue, 26 Aug 1997 19:02:32 +0100 (BST)
--* Received: from sp2-2.scri.fsu.edu (sp2-2.scri.fsu.edu [144.174.128.92]) by mailer.scri.fsu.edu (8.8.5/8.7.5) with SMTP id OAA27332; Tue, 26 Aug 1997 14:00:41 -0400 (EDT)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by sp2-2.scri.fsu.edu (5.67b) id AA18324; Tue, 26 Aug 1997 14:00:40 -0400
--* Date: Tue, 26 Aug 1997 14:00:40 -0400
--* Message-Id: <199708261800.AA18324@sp2-2.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs@nag.co.uk, edwards@scri.fsu.edu,
--*         youssef@scri.fsu.edu
--* Subject: [4] category defaults with conditionals

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.9a
-- Original bug file name: bug0.as

--+ --
--+ --  I'm trying to define the category PartialOrder below.  In 
--+ --  general, this isn't a BasicType and just has the signatures
--+ --  <= and >=.  However, if is a BasicType, I would like to 
--+ --  supply the additional operations > and <.  I can also supply
--+ --  these by default, but this doesn't seem to work and the 
--+ --  compiler complains that there isn't any "=" defined, in
--+ --  spite of the "if % has BasicType ..."  
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ --define PartialOrder:Category == BasicType with -- use this instead and there's no error
--+ define PartialOrder:Category == with 
--+   <=:(%,%) -> Boolean
--+   >=:(%,%) -> Boolean
--+   if % has BasicType then
--+     < :(%,%) -> Boolean
--+     > :(%,%) -> Boolean
--+     
--+   default
--+     >=(a:%,b:%):Boolean == b<=a
--+     if % has BasicType then
--+       < (a:%,b:%):Boolean == a<=b and not (a=b)
--+       > (a:%,b:%):Boolean == a>=b and not (a=b)
--+     
--+ #endpile
--+   
--+   
--
--  I'm trying to define the category PartialOrder below.  In 
--  general, this isn't a BasicType and just has the signatures
--  <= and >=.  However, if is a BasicType, I would like to 
--  supply the additional operations > and <.  I can also supply
--  these by default, but this doesn't seem to work and the 
--  compiler complains that there isn't any "=" defined, in
--  spite of the "if % has BasicType ..."  
--
#include "axllib"
#pile

--define PartialOrder:Category == BasicType with -- use this instead and there's no error
define PartialOrder:Category == with 
  <=:(%,%) -> Boolean
  >=:(%,%) -> Boolean
  if % has BasicType then
    < :(%,%) -> Boolean
    > :(%,%) -> Boolean
    
  default
    >=(a:%,b:%):Boolean == b<=a
    if % has BasicType then
      < (a:%,b:%):Boolean == a<=b and not (a=b)
      > (a:%,b:%):Boolean == a>=b and not (a=b)
    
#endpile
  
  
