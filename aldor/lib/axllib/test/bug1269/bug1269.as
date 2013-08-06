--* From youssef@d0mino.fnal.gov  Sun Oct 22 03:14:12 2000
--* Received: from server-6.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id DAA29059
--* 	for <ax-bugs@nag.co.uk>; Sun, 22 Oct 2000 03:14:11 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 31801 invoked from network); 22 Oct 2000 02:13:40 -0000
--* Received: from d0mino.fnal.gov (131.225.224.45)
--*   by server-6.tower-4.starlabs.net with SMTP; 22 Oct 2000 02:13:40 -0000
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id VAA17763;
--* 	Sat, 21 Oct 2000 21:13:37 -0500 (CDT)
--* Date: Sat, 21 Oct 2000 21:13:37 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200010220213.VAA17763@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: domaindomain.as

--+ --
--+ --  Hi Martin,
--+ --
--+ --     Here's a real fundamental problem or inconsistency or something that
--+ --  I've noticed.  It concerns non-constant Domains and the warning that
--+ --  one sometimes gets:
--+ --
--+ --  "(Warning) Function returns a domain that might not be constant"
--+ --
--+ --  I have learned by experience to take this warning seriously.  Whenever
--+ --  I've gotten this warning, there is always a problem later on, usually 
--+ --  a compiler core dump when using the domain.  I've marked each domain 
--+ --  constructor that generates this warning below.
--+ --
--+ --  It seems to me that there are several problems with the way this works
--+ --  now:
--+ --
--+ --   (a) There seems to be no way to satisfy the F5 signature in FooDom
--+ --       without generating the warning (and a later core dump, in my 
--+ --       experience anyway).  If you do "== Domain add" for F5, it doesn't
--+ --       satisfy the signature.  This problem limits what kind of 
--+ --       signatures you can have in a domain.  Is there some way around this?
--+ --
--+ --   (b) If F5: Category -> with is not supposed to be done because it
--+ --       makes a non-constant domain, why does F5Outside compile without
--+ --       a warning?  In my experience, domain constructors like F5Outside
--+ --       work just fine.  
--+ --
--+ --   (c) Why is there a difference between F2Outside..F5Outside and 
--+ --       G2Outside..G5Outside?  The compiler clearly recognizes that
--+ --       its a domain constructor in both cases.
--+ --
--+ --   Cheers,   Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ Domain:with == add
--+ 
--+ FooDom:with
--+     F1:                  with
--+     F2:            () -> with
--+     F3: SingleInteger -> with
--+     F4:          Type -> with
--+     F5:      Category -> with
--+ == add
--+     F1                   :with == Domain
--+     F2()                 :with == Domain           -- warning
--+     F3(x:SingleInteger)  :with == Domain           -- warning
--+     F4(Obj:Type)         :with == Domain           -- warning
--+     F5(Obj:Category)     :with == Domain           -- warning
--+ 
--+ F1Outside                   :with == Domain
--+ F2Outside()                 :with == Domain add
--+ F3Outside(x:SingleInteger)  :with == Domain add
--+ F4Outside(Obj:Type)         :with == Domain add
--+ F5Outside(Obj:Category)     :with == Domain add    
--+ 
--+ G1Outside                   :with == Domain
--+ G2Outside()                 :with == Domain        -- warning
--+ G3Outside(x:SingleInteger)  :with == Domain        -- warning
--+ G4Outside(Obj:Type)         :with == Domain        -- warning
--+ G5Outside(Obj:Category)     :with == Domain        -- warning
--+ 
--
--  Hi Martin,
--
--     Here's a real fundamental problem or inconsistency or something that
--  I've noticed.  It concerns non-constant Domains and the warning that
--  one sometimes gets:
--
--  "(Warning) Function returns a domain that might not be constant"
--
--  I have learned by experience to take this warning seriously.  Whenever
--  I've gotten this warning, there is always a problem later on, usually 
--  a compiler core dump when using the domain.  I've marked each domain 
--  constructor that generates this warning below.
--
--  It seems to me that there are several problems with the way this works
--  now:
--
--   (a) There seems to be no way to satisfy the F5 signature in FooDom
--       without generating the warning (and a later core dump, in my 
--       experience anyway).  If you do "== Domain add" for F5, it doesn't
--       satisfy the signature.  This problem limits what kind of 
--       signatures you can have in a domain.  Is there some way around this?
--
--   (b) If F5: Category -> with is not supposed to be done because it
--       makes a non-constant domain, why does F5Outside compile without
--       a warning?  In my experience, domain constructors like F5Outside
--       work just fine.  
--
--   (c) Why is there a difference between F2Outside..F5Outside and 
--       G2Outside..G5Outside?  The compiler clearly recognizes that
--       its a domain constructor in both cases.
--
--   Cheers,   Saul
--
#include "axllib"
#pile

Domain:with == add

FooDom:with
    F1:                  with
    F2:            () -> with
    F3: SingleInteger -> with
    F4:          Type -> with
    F5:      Category -> with
== add
    F1                   :with == Domain
    F2()                 :with == Domain           -- warning
    F3(x:SingleInteger)  :with == Domain           -- warning
    F4(Obj:Type)         :with == Domain           -- warning
    F5(Obj:Category)     :with == Domain           -- warning

F1Outside                   :with == Domain
F2Outside()                 :with == Domain add
F3Outside(x:SingleInteger)  :with == Domain add
F4Outside(Obj:Type)         :with == Domain add
F5Outside(Obj:Category)     :with == Domain add    

G1Outside                   :with == Domain
G2Outside()                 :with == Domain        -- warning
G3Outside(x:SingleInteger)  :with == Domain        -- warning
G4Outside(Obj:Type)         :with == Domain        -- warning
G5Outside(Obj:Category)     :with == Domain        -- warning


