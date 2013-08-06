--* From youssef@d0mino.fnal.gov  Fri Sep  8 11:42:00 2000
--* Received: from d0mino.fnal.gov (d0mino.fnal.gov [131.225.224.45])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id LAA22435
--* 	for <ax-bugs@nag.co.uk>; Fri, 8 Sep 2000 11:41:56 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id FAA77080;
--* 	Fri, 8 Sep 2000 05:41:45 -0500 (CDT)
--* Date: Fri, 8 Sep 2000 05:41:45 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200009081041.FAA77080@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: environment.as

--+ --
--+ --  Hi Martin,
--+ --     You can see the problem in the last line of this file.  The domain RngFunctor is 
--+ --  required to supply an export called "functor".  Although it supplies no exports, it
--+ --  still compiles without an error "axiomxl -g interp".  If you comment out the "functor"
--+ --  function in this file it will be fairly clear that it's picking up this as the 
--+ --  required export.  I'm pretty sure that this isn't meant to happen. 
--+ --
--+ --  Cheers,
--+ --
--+ --    Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 
--+ define Printable:Category == with
--+     <<: (TextWriter,%) -> TextWriter   
--+ 
--+ define MapCategory(Object:Category,A:Object,B:Object):Category == with
--+     apply: (%,A)  -> B
--+     mor:   (A->B) -> %
--+ 
--+ Map(Object:Category,A:Object,B:Object):MapCategory(Object,A,B) == add
--+     Rep ==> (A->B)
--+     apply(f:%,a:A):B == (rep f) a
--+     mor(f:A->B):% == per f
--+ 
--+ define Rng:Category == Printable with
--+     +: (%,%) -> %
--+     *: (%,%) -> %
--+     1:          %
--+     0:          %
--+     
--+ define RngMorphism(A:Rng,B:Rng):Category ==  Rng with MapCategory(Rng,A,B)
--+ 
--+ Mor(A:Rng,B:Rng):RngMorphism(A,B) == Map(Rng,A,B) add
--+     (f:%)+(g:%):% == mor ( (a:A):B +-> f a + g a )
--+     (f:%)*(g:%):% == mor ( (a:A):B +-> f a * g a )
--+     1:% == mor ( (a:A):B +-> 1 )
--+     0:% == mor ( (a:A):B +-> 0 )
--+     (t:TextWriter)<<(f:%):TextWriter == t << "[RngMorphism]"
--+ 
--+ Fo(A:Rng):Rng == add
--+     Rep ==> Record(a:A,b:A); import from Rep
--+     
--+     (x:%)+(y:%):% == per [ rep(x).a + rep(y).a, rep(x).b + rep(y).b ]
--+     (x:%)*(y:%):% == per [ rep(x).a * rep(y).a, rep(x).b * rep(y).b ]
--+     0:%           == per [ 0, 0 ]
--+     1:%           == per [ 1, 1 ]
--+     (t:TextWriter)<<(x:%):TextWriter == t << "(" << rep(x).a << "," << rep(x).b << ")"
--+     
--+ Fm(A:Rng,B:Rng,f:Mor(A,B)):Mor(Fo A,Fo B) == 
--+     import from Record(a:A,b:A),Record(a:B,b:B)
--+     p1(x:Fo A):A == (x pretend Record(a:A,b:A)).a
--+     p2(x:Fo A):A == (x pretend Record(a:A,b:A)).b
--+     mor ( (x:Fo A):Fo B +->  [f p1 x,f p2 x] pretend Fo B )
--+         
--+ define Functor(Obj:Type,Mc:(Obj,Obj)->Category,Md:(A:Obj,B:Obj)->Mc(A,B)):Category == with
--+     functor: (A:Obj,B:Obj,f:Md(A,B))->(FA:Obj,FB:Obj,Md(FA,FB))
--+ --
--+ --  comment the following line and RngFunctor will have a missing export
--+ --
--+ functor(A:Rng,B:Rng,f:Mor(A,B)):(FA:Rng,FB:Rng,Mor(FA,FB)) == (Fo A,Fo B,Fm(A,B,f))
--+     
--+ RngFunctor:Functor(Rng,RngMorphism,Mor) == add -- picks up the "functor" function above
--+ 
--
--  Hi Martin,
--     You can see the problem in the last line of this file.  The domain RngFunctor is 
--  required to supply an export called "functor".  Although it supplies no exports, it
--  still compiles without an error "axiomxl -g interp".  If you comment out the "functor"
--  function in this file it will be fairly clear that it's picking up this as the 
--  required export.  I'm pretty sure that this isn't meant to happen. 
--
--  Cheers,
--
--    Saul
--
#include "axllib"
#pile

define Printable:Category == with
    <<: (TextWriter,%) -> TextWriter   

define MapCategory(Object:Category,A:Object,B:Object):Category == with
    apply: (%,A)  -> B
    mor:   (A->B) -> %

Map(Object:Category,A:Object,B:Object):MapCategory(Object,A,B) == add
    Rep ==> (A->B)
    apply(f:%,a:A):B == (rep f) a
    mor(f:A->B):% == per f

define Rng:Category == Printable with
    +: (%,%) -> %
    *: (%,%) -> %
    1:          %
    0:          %
    
define RngMorphism(A:Rng,B:Rng):Category ==  Rng with MapCategory(Rng,A,B)

Mor(A:Rng,B:Rng):RngMorphism(A,B) == Map(Rng,A,B) add
    (f:%)+(g:%):% == mor ( (a:A):B +-> f a + g a )
    (f:%)*(g:%):% == mor ( (a:A):B +-> f a * g a )
    1:% == mor ( (a:A):B +-> 1 )
    0:% == mor ( (a:A):B +-> 0 )
    (t:TextWriter)<<(f:%):TextWriter == t << "[RngMorphism]"

Fo(A:Rng):Rng == add
    Rep ==> Record(a:A,b:A); import from Rep
    
    (x:%)+(y:%):% == per [ rep(x).a + rep(y).a, rep(x).b + rep(y).b ]
    (x:%)*(y:%):% == per [ rep(x).a * rep(y).a, rep(x).b * rep(y).b ]
    0:%           == per [ 0, 0 ]
    1:%           == per [ 1, 1 ]
    (t:TextWriter)<<(x:%):TextWriter == t << "(" << rep(x).a << "," << rep(x).b << ")"
    
Fm(A:Rng,B:Rng,f:Mor(A,B)):Mor(Fo A,Fo B) == 
    import from Record(a:A,b:A),Record(a:B,b:B)
    p1(x:Fo A):A == (x pretend Record(a:A,b:A)).a
    p2(x:Fo A):A == (x pretend Record(a:A,b:A)).b
    mor ( (x:Fo A):Fo B +->  [f p1 x,f p2 x] pretend Fo B )
        
define Functor(Obj:Type,Mc:(Obj,Obj)->Category,Md:(A:Obj,B:Obj)->Mc(A,B)):Category == with
    functor: (A:Obj,B:Obj,f:Md(A,B))->(FA:Obj,FB:Obj,Md(FA,FB))
--
--  comment the following line and RngFunctor will have a missing export
--
functor(A:Rng,B:Rng,f:Mor(A,B)):(FA:Rng,FB:Rng,Mor(FA,FB)) == (Fo A,Fo B,Fm(A,B,f))
    
RngFunctor:Functor(Rng,RngMorphism,Mor) == add -- picks up the "functor" function above

