--* From youssef@mailer.scri.fsu.edu  Mon Jul  7 22:36:01 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA11572; Mon, 7 Jul 97 22:36:01 +0100
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id WAA10012 for <ax-bugs@nag.co.uk>; Mon, 7 Jul 1997 22:36:03 +0100 (BST)
--* Received: from sp2-3.scri.fsu.edu (sp2-3.scri.fsu.edu [144.174.128.93]) by mailer.scri.fsu.edu (8.8.5/8.7.5) with SMTP id RAA29232; Mon, 7 Jul 1997 17:34:36 -0400 (EDT)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by sp2-3.scri.fsu.edu (5.67b) id AA13419; Mon, 7 Jul 1997 17:34:35 -0400
--* Date: Mon, 7 Jul 1997 17:34:35 -0400
--* Message-Id: <199707072134.AA13419@sp2-3.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs@nag.co.uk, edwards@scri.fsu.edu
--* Subject: [2] Compiler crashes

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -v -g interp file.as
-- Version: 1.1.9a
-- Original bug file name: bug.as

--+ 
--+ Greetings,
--+ 
--+    On our aix machines, % axiomxl -v -g interp bug.as causes
--+ the compiler to have a segmentation fault.
--+ 
--+   Saul Youssef
--+   SCRI
--+   
--+   
#pile
#include "axllib"
  
TMS ==> TopologicalMeasureSpace
SI  ==> SingleInteger

define Topology: Category == BasicType with
  intersection: (%,%) -> %  ++ Open set intersection
  whole:                 %  ++ Whole space open set
  empty:                 %  ++ Empty open set
  empty?:     % -> Boolean  ++ Tests whether an open set is empty
  
define Module(R:Ring): Category == AbelianGroup with 
  *:   (R,%)-> %             ++ Ring-scalar*Vector multiplication
  
define MeasureSpace(R:Ring): Category == BasicType with
  m:            % -> R        ++ Measure
  divide:       % -> Array %  ++ A partition of the given open set
  
define TopologicalMeasureSpace(R:Ring): Category == Topology with MeasureSpace(R)

DirectProductTopology2(R:Ring)(T1:TMS(R),T2:TMS(R)):TMS(R)
  with 
    #:SI                    ++ Number of components in the direct product
    new:(T1,T2)->%          ++ Constructor
    bracket:(T1,T2)->%      ++ Constructor
  == add
    Rep ==> Record(d1:T1,d2:T2)
    import from Rep, Array T1, Array T2
  
    #:SI == 2
    if #=0 then error "Attempt to construct an empty direct product"
    
    (a:%)=(b:%):Boolean == rep a = rep b
    sample: % == per [sample$T1, sample$T2]
    intersection(a:%,b:%):% ==
       per [intersection(rep(a).d1,rep(b).d1), intersection(rep(a).d2,rep(b).d2)]     
    m(a:%):R == m(rep(a).d1)*m(rep(a).d2)
  
    whole:% == per [whole$T1,whole$T2]
    empty:% == per [empty$T1,empty$T2]
    empty?(a:%):Boolean == empty?(rep(a).d1) or empty?(rep(a).d2)
  
    <<(t:TextWriter,a:%):TextWriter == t << "DirectProductTopology2(" 
      << rep(a).d1 << "," << rep(a).d2 << ")"
      
    new(t1:T1,t2:T2):%     == per [t1,t2]
    bracket(t1:T1,t2:T2):%  == per [t1,t2]
      
    compindex:Generator SI == generate { repeat{yield 1; yield 2} }
    divide(a:%):Array % ==
      i := value step! compindex
      i=1 =>
        u := divide(rep(a).d1)
        #u=1 => [per [u.1,rep(a).d2]]
        #u=2 => [per [u.1,rep(a).d2] , per[u.2,rep(a).d2]]
        never
      i=2 =>
        v := divide(rep(a).d2)
        #v=1 => [per [rep(a).d1,v.1]]
        #v=2 => [per [rep(a).d1,v.1] , per[rep(a).d1,v.2]]
        never
      never
--
-- Integration package
--
Integration: with 
    integrate:(R:Ring,X:TMS(R),O:X==whole$X, f:X->Y, Y:Join(Topology,Module(R)))->Generator Y
  == add
    integrate (R:Ring,X:TMS(R),O:X==whole$X, f:X->Y, Y:Join(Topology,Module(R))):Generator Y ==
      import from Array X
      
      generate
        sum:Y := m(O)*f(O)
        OL:List X := [O]
        yield sum
        
        while ~empty?(OL) repeat
          A := divide first OL
          sum := sum - m(first OL)*f(first OL)
          for a in A repeat sum := sum + m(a)*f(a)
          OL := intListUpdate(X,A,OL)
          
          yield sum
          
    intListUpdate(X:BasicType,A:Array X,OL:List X):List X ==
      L: List X := empty()
      for a in A repeat L := cons(a,L)
      
      L := concat(L, rest OL)
      
      if X has Order then
        import from ListSort(X)
        L := sort(>$X,L)
      L

TestTop(R:Ring): TopologicalMeasureSpace(R) with {Module(R); new:(R,R)->%; 
  low:% -> R; high:%->R} == add
  Rep ==> Record(low:R, high:R)
  import from Rep
  
  new(low:R,high:R):% ==  per [low,high]
  empty: % == per [0,0]
  empty?(o:%):Boolean == rep(o).low = rep(o).high
  
  whole: % == per [0, 0] -- error "not written yet"
  
  intersection(o1:%,o2:%):% == per [0, 0] -- error "not written yet"
  
  =(o1:%,o2:%):Boolean == (empty?(o1) and empty?(o2)) or 
     ((rep(o1).low = rep(o2).low) and (rep(o1).high = rep(o2).high))
     
  sample: % == per [0,0]
  m(o:%):R == (rep(o).high - rep(o).low)
  
  low(o:%):R == rep(o).low
  high(o:%):R == rep(o).high
  
  <<(t:TextWriter,o:%):TextWriter == t << "Open Set(" << o << ")"
  
  divide(o:%):Array % == 
    empty? o => [o]
    mid := rep(o).low + 1
    [per[rep(o).low,mid], per[mid,rep(o).high]]

  (o1:%)+(o2:%):%  == per [rep(o1).low+rep(o2).low, rep(o1).high+rep(o2).high]
  (o1:%)-(o2:%):%  == per [rep(o1).low-rep(o2).low, rep(o1).high-rep(o2).high]
  (x:R)*(o:%):%   == per [x*rep(o).low, x*rep(o).high]
  -(o:%):% == per [-rep(o).high,-rep(o).low]
  0:% == per [0,0]
  
ff(o:TestTop(SI)):TestTop(SI) == 
  import from SI
  new(high(o)-low(o),high(o)-low(o)+1)
  
if SI has Ring then print << "OK" << newline
if TestTop(SI) has TMS(SI) then  print << "OK2" << newline

--import from Integration,TestTop
--g := integrate(SI,TestTop,whole$TestTop,ff,TestTop)  

#endpile
  
