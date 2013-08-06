--* From youssef@d0mino.fnal.gov  Sat Nov 11 01:48:45 2000
--* Received: from server-11.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id BAA17724
--* 	for <ax-bugs@nag.co.uk>; Sat, 11 Nov 2000 01:48:44 GMT
--* X-VirusChecked: Checked
--* Received: (qmail 21281 invoked from network); 11 Nov 2000 01:48:12 -0000
--* Received: from d0mino.fnal.gov (131.225.224.45)
--*   by server-11.tower-4.starlabs.net with SMTP; 11 Nov 2000 01:48:12 -0000
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id TAA59165;
--* 	Fri, 10 Nov 2000 19:48:10 -0600 (CST)
--* Date: Fri, 10 Nov 2000 19:48:10 -0600 (CST)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200011110148.TAA59165@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: incomplete_domain.as

--+ --
--+ --  Hi Martin,
--+ --
--+ --      Guess who?  Here's something that might be isolating a tricky bug.  If you execute the code below, you will
--+ --  get an error 
--+ --
--+ --    "hash value read on incomplete domain"
--+ --
--+ --  Mysteriously, if you comment the three lines indicating, removing the unused "One" and "one" signatures, you get 
--+ --  one of those "Export not found" errors failing to find "Product."   Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ define MapCategory(Obj:Category,A:Obj,B:Obj):Category == with
--+     apply: (%, A) -> B
--+     mor:   (A->B) -> %
--+ 
--+ Map(Obj:Category,A:Obj,B:Obj):MapCategory(Obj,A,B) == add
--+     Rep ==> A->B
--+     apply(f:%,a:A):B == (rep f) a 
--+     mor  (f:A->B):% == per f
--+     
--+ define Product(Obj:Type,Mor:(Obj,Obj)->with):Category == with
--+     Product: (A:Obj,B:Obj) -> (Prod:Obj, (X:Obj)->(Mor(X,A),Mor(X,B))->Mor(X,Prod))  
--+   
--+ define Agg:Category == with
--+ define AggMorphism(A:Agg,B:Agg):Category == MapCategory(Agg,A,B) with
--+ Agg(A:Agg,B:Agg):AggMorphism(A,B) == Map(Agg,A,B) add
--+ 
--+ AggCategory:Product(Agg,Agg) _
--+                                   with { One:Agg; one: (A:Agg)->Agg(A,One) }        -- Comment this line...
--+ == add
--+     One:Agg ==  ( add { Rep == SingleInteger; import from Rep } ) @ Agg             -- ...and this one
--+     one(A:Agg):Agg(A,One) == mor ( (a:A):One +-> (1$SingleInteger) pretend One )    -- ...and this one to go from "incomplete domain" to "export not found"
--+ 
--+     Product(A:Agg,B:Agg):(Prod:Agg,(X:Agg)->(Agg(X,A),Agg(X,B))->Agg(X,Prod)) ==
--+         ProdRep == Record(a:A,b:B); import from ProdRep
--+         P:Agg == add
--+         product(X:Agg)(f:Agg(X,A),g:Agg(X,B)):Agg(X,P) ==
--+             import from Record(a:A,b:B)
--+             mor ( (x:X):P +-> [f x,g x] pretend P )    
--+         (P,product)
--+ 
--+ IntAgg:Agg == Integer add
--+ import from AggCategory
--+ 
--+ (IntAgg2:Agg,product) == Product(IntAgg,IntAgg)
--
--  Hi Martin,
--
--      Guess who?  Here's something that might be isolating a tricky bug.  If you execute the code below, you will
--  get an error 
--
--    "hash value read on incomplete domain"
--
--  Mysteriously, if you comment the three lines indicating, removing the unused "One" and "one" signatures, you get 
--  one of those "Export not found" errors failing to find "Product."   Saul
--
#include "axllib"
#pile

define MapCategory(Obj:Category,A:Obj,B:Obj):Category == with
    apply: (%, A) -> B
    mor:   (A->B) -> %

Map(Obj:Category,A:Obj,B:Obj):MapCategory(Obj,A,B) == add
    Rep ==> A->B
    apply(f:%,a:A):B == (rep f) a 
    mor  (f:A->B):% == per f
    
define Product(Obj:Type,Mor:(Obj,Obj)->with):Category == with
    Product: (A:Obj,B:Obj) -> (Prod:Obj, (X:Obj)->(Mor(X,A),Mor(X,B))->Mor(X,Prod))  
  
define Agg:Category == with
define AggMorphism(A:Agg,B:Agg):Category == MapCategory(Agg,A,B) with
Agg(A:Agg,B:Agg):AggMorphism(A,B) == Map(Agg,A,B) add

AggCategory:Product(Agg,Agg) _
                                  with { One:Agg; one: (A:Agg)->Agg(A,One) }        -- Comment this line...
== add
    One:Agg ==  ( add { Rep == SingleInteger; import from Rep } ) @ Agg             -- ...and this one
    one(A:Agg):Agg(A,One) == mor ( (a:A):One +-> (1$SingleInteger) pretend One )    -- ...and this one to go from "incomplete domain" to "export not found"

    Product(A:Agg,B:Agg):(Prod:Agg,(X:Agg)->(Agg(X,A),Agg(X,B))->Agg(X,Prod)) ==
        ProdRep == Record(a:A,b:B); import from ProdRep
        P:Agg == add
        product(X:Agg)(f:Agg(X,A),g:Agg(X,B)):Agg(X,P) ==
            import from Record(a:A,b:B)
            mor ( (x:X):P +-> [f x,g x] pretend P )    
        (P,product)

IntAgg:Agg == Integer add
import from AggCategory

(IntAgg2:Agg,product) == Product(IntAgg,IntAgg)

