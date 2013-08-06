--* From youssef@mailer.scri.fsu.edu  Fri Oct  9 23:12:21 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA17839; Fri, 9 Oct 98 23:12:21 +0100
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id XAA16296 for <ax-bugs@nag.co.uk>; Fri, 9 Oct 1998 23:15:59 +0100 (BST)
--* Received: from dirac.scri.fsu.edu (dirac.scri.fsu.edu [144.174.128.44]) by mailer.scri.fsu.edu (8.8.7/8.7.5) with SMTP id SAA28000 for <ax-bugs@nag.co.uk>; Fri, 9 Oct 1998 18:12:50 -0400 (EDT)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by dirac.scri.fsu.edu (5.67b) id AA81774; Fri, 9 Oct 1998 18:12:46 -0400
--* Date: Fri, 9 Oct 1998 18:12:46 -0400
--* Message-Id: <199810092212.AA81774@dirac.scri.fsu.edu>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] minor bug with category defaults

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp file.as
-- Version: 1.1.11e
-- Original bug file name: bug0.as

--+ -- 
--+ -- I believe that this demonstrates a minor bug with defaults.  
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ define FooCat:Category == with
--+     f: % ->% 
--+ --    g: % ->%   -- this works even with g taken out of FooCat
--+     default 
--+         f(x:%):% == x
--+         g(x:%):% == x
--+         
--+ FooDom:FooCat == add
--+ 
--+ ff(x:FooDom):FooDom == g x 
--+ 
-- 
-- I believe that this demonstrates a minor bug with defaults.  
--
#include "axllib"
#pile

define FooCat:Category == with
    f: % ->% 
--    g: % ->%   -- this works even with g taken out of FooCat
    default 
        f(x:%):% == x
        g(x:%):% == x
        
FooDom:FooCat == add

ff(x:FooDom):FooDom == g x 

