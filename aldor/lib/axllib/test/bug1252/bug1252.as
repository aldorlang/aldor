--* From youssef@d0mino.fnal.gov  Wed Sep  6 06:26:03 2000
--* Received: from d0mino.fnal.gov (d0mino.fnal.gov [131.225.224.45])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id GAA20063
--* 	for <ax-bugs@nag.co.uk>; Wed, 6 Sep 2000 06:25:57 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0mino.fnal.gov (SGI-8.9.3/8.9.3) id AAA96198;
--* 	Wed, 6 Sep 2000 00:25:54 -0500 (CDT)
--* Date: Wed, 6 Sep 2000 00:25:54 -0500 (CDT)
--* From: Saul Youssef <youssef@d0mino.fnal.gov>
--* Message-Id: <200009060525.AAA96198@d0mino.fnal.gov>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.12p6
-- Original bug file name: export.as

--+ --
--+ -- Martin,
--+ --
--+ --    Here's another problem.  If you compile this with axiomxl -g interp, you should
--+ -- get the message:
--+ --
--+ --   Looking in Fun for functor with code 459131498
--+ --   Unhandled Exception: RuntimeError(??)
--+ --   Export not found
--+ --
--+ -- However, "functor" looks well defined to me here.  Can you tell if there is something
--+ -- wrong?
--+ --
--+ --   Saul
--+ --
--+ #include "axllib"
--+ #pile
--+ 	  
--+ define MapCategory(Object:Category,A:Object,B:Object):Category == with
--+     apply: (%,A) -> B
--+     mor:   (A->B)-> %
--+     
--+ Map(Object:Category,A:Object,B:Object):MapCategory(Object,A,B) == add
--+     Rep ==> (A->B)
--+     apply(f:%,a:A):B == (rep f) a
--+     mor(f:A->B):% == per f
--+ 
--+ define Rng:Category == with 
--+     +: (%,%) -> %
--+     *: (%,%) -> %
--+     1:          %
--+     0:          %
--+     
--+ define RngMorphism(A:Rng,B:Rng):Category == Rng with MapCategory(Rng,A,B)
--+ 
--+ Mor(A:Rng,B:Rng):RngMorphism(A,B) == Map(Rng,A,B) add
--+     (f:%)+(g:%):% == mor ( (a:A):B +-> f a + g a )
--+     (f:%)*(g:%):% == mor ( (a:A):B +-> f a * g a )
--+     1:% == mor ( (a:A):B +-> 1 )
--+     0:% == mor ( (a:A):B +-> 0 )
--+     
--+ define Functor(O1:Type,Mc1:(O1,O1)->Category,Md1:(A:O1,B:O1)->Mc1(A,B),_
--+                O2:Type,Mc2:(O2,O2)->Category,Md2:(A:O2,B:O2)->Mc2(A,B)):Category == with
--+     functor: (A:O1,B:O1,Md1(A,B))->(FA:O2,FB:O2,Md2(FA,FB))
--+ 
--+ Fo(A:Rng):Rng == add
--+     Rep ==> Record(a:A,b:A); import from Rep
--+     
--+     (x:%)+(y:%):% == per [ rep(x).a + rep(y).a, rep(x).b + rep(y).b ]
--+     (x:%)*(y:%):% == per [ rep(x).a * rep(y).a, rep(x).b * rep(y).b ]
--+     0:%           == per [ 0, 0 ]
--+     1:%           == per [ 1, 1 ]
--+     
--+ Fm(A:Rng,B:Rng,f:Mor(A,B)):Mor(Fo A,Fo B) == 
--+     import from Record(a:A,b:A)
--+     p1(x:Fo A):A == (x pretend Record(a:A,b:A)).a
--+     p2(x:Fo A):A == (x pretend Record(a:A,b:A)).b
--+     mor ( (x:Fo A):Fo B +->  [p1 x,p2 x] pretend Fo B )
--+ 
--+ Fun:Functor(Rng,RngMorphism,Mor, Rng,RngMorphism,Mor) == add
--+     functor(A:Rng,B:Rng,f:Mor(A,B)):(FA:Rng,FB:Rng,Mor(FA,FB)) == (Fo A,Fo B,Fm(A,B,f))
--+ 
--+ import from Fun
--+ 
--+ IntegerRng:Rng == add
--+     Rep ==> SingleInteger; import from Rep
--+     (x:%)+(y:%):% == per ( rep x + rep y)
--+     (x:%)*(y:%):% == per ( rep x * rep y)
--+     1:% == per 1
--+     0:% == per 0
--+     
--+ m:Mor(IntegerRng,IntegerRng) == mor ( (x:IntegerRng):IntegerRng +-> x + x )
--+ 
--+ (A,B,m2) == functor ( IntegerRng, IntegerRng, m )
--
-- Martin,
--
--    Here's another problem.  If you compile this with axiomxl -g interp, you should
-- get the message:
--
--   Looking in Fun for functor with code 459131498
--   Unhandled Exception: RuntimeError(??)
--   Export not found
--
-- However, "functor" looks well defined to me here.  Can you tell if there is something
-- wrong?
--
--   Saul
--
#include "axllib"
#pile
	  
define MapCategory(Object:Category,A:Object,B:Object):Category == with
    apply: (%,A) -> B
    mor:   (A->B)-> %
    
Map(Object:Category,A:Object,B:Object):MapCategory(Object,A,B) == add
    Rep ==> (A->B)
    apply(f:%,a:A):B == (rep f) a
    mor(f:A->B):% == per f

define Rng:Category == with 
    +: (%,%) -> %
    *: (%,%) -> %
    1:          %
    0:          %
    
define RngMorphism(A:Rng,B:Rng):Category == Rng with MapCategory(Rng,A,B)

Mor(A:Rng,B:Rng):RngMorphism(A,B) == Map(Rng,A,B) add
    (f:%)+(g:%):% == mor ( (a:A):B +-> f a + g a )
    (f:%)*(g:%):% == mor ( (a:A):B +-> f a * g a )
    1:% == mor ( (a:A):B +-> 1 )
    0:% == mor ( (a:A):B +-> 0 )
    
define Functor(O1:Type,Mc1:(O1,O1)->Category,Md1:(A:O1,B:O1)->Mc1(A,B),_
               O2:Type,Mc2:(O2,O2)->Category,Md2:(A:O2,B:O2)->Mc2(A,B)):Category == with
    functor: (A:O1,B:O1,Md1(A,B))->(FA:O2,FB:O2,Md2(FA,FB))

Fo(A:Rng):Rng == add
    Rep ==> Record(a:A,b:A); import from Rep
    
    (x:%)+(y:%):% == per [ rep(x).a + rep(y).a, rep(x).b + rep(y).b ]
    (x:%)*(y:%):% == per [ rep(x).a * rep(y).a, rep(x).b * rep(y).b ]
    0:%           == per [ 0, 0 ]
    1:%           == per [ 1, 1 ]
    
Fm(A:Rng,B:Rng,f:Mor(A,B)):Mor(Fo A,Fo B) == 
    import from Record(a:A,b:A)
    p1(x:Fo A):A == (x pretend Record(a:A,b:A)).a
    p2(x:Fo A):A == (x pretend Record(a:A,b:A)).b
    mor ( (x:Fo A):Fo B +->  [p1 x,p2 x] pretend Fo B )

Fun:Functor(Rng,RngMorphism,Mor, Rng,RngMorphism,Mor) == add
    functor(A:Rng,B:Rng,f:Mor(A,B)):(FA:Rng,FB:Rng,Mor(FA,FB)) == (Fo A,Fo B,Fm(A,B,f))

import from Fun

IntegerRng:Rng == add
    Rep ==> SingleInteger; import from Rep
    (x:%)+(y:%):% == per ( rep x + rep y)
    (x:%)*(y:%):% == per ( rep x * rep y)
    1:% == per 1
    0:% == per 0
    
m:Mor(IntegerRng,IntegerRng) == mor ( (x:IntegerRng):IntegerRng +-> x + x )

(A,B,m2) == functor ( IntegerRng, IntegerRng, m )
