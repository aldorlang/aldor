--> testint
--> testrun -l axllib
--> testrun -O -l axllib
--> testerrs
macro {
        rep x == x @ % pretend Rep;
        per r == r @ Rep pretend %;
}

macro {
        BBool == Bool$Machine;
        BChar == Char$Machine;
        BArr  == Arr$Machine;
        BPtr  == Ptr$Machine;
        BByte == XByte$Machine;
        BHInt == HInt$Machine;
        BSInt == SInt$Machine;
        BBInt == BInt$Machine;
        BSFlo == SFlo$Machine;
        BDFlo == DFlo$Machine;
}





#	library AxlLib "axllib"
{
        import from AxlLib;
        inline from AxlLib;
}

{
        import from Boolean;
}

{
        import {
                string:	Literal -> %;
                <<:	(TextWriter, %) -> TextWriter;
                <<:	% -> TextWriter -> TextWriter;
        } from String;
}

{
        import {
                newline: %;
                <<:	(TextWriter, %) -> TextWriter;
                <<:	% -> TextWriter -> TextWriter;
        } from Character;
}

{
        import {
                print:	%;
                errout:	%;
        } from TextWriter;
}

{
        import from FormattedOutput;
}

#pile

S ==> SingleInteger

define FloatCategory: Category == OrderedRing with 
    step: S -> (%,%) -> Generator %
    exp:           % -> %
    float:   Literal -> %
    
SDoubleFloat: FloatCategory == DoubleFloat add
    Rep ==> DoubleFloat
    import from Rep, DoubleFloatElementaryFunctions
    
    exp(x:%):%         == per exp rep x
    float(l:Literal):% == per float l
    
SFloat: FloatCategory == Float add 
    Rep ==> Float

NTuple(F:FloatCategory,n:S): OrderedRing with
    ntuple: Array F   ->  %        -- create a new ntuple
    ntuple: Tuple F   ->  %
    val:          %   ->  Array F  -- return n-tuple as an array
    apply:    (%,S)   ->  F        -- return n-tuple component
    set!:   (%,S,F)   ->  %
    dim:          %   ->  S        -- dimension of the n-tuple
    mag2:         %   ->  F        -- sum of squares of components
    sum:          %   ->  F        -- sum of components
    *:        (F,%)   ->  %        -- scalar muliplication
== add
    Rep ==> Array F
    import from S, Rep
    ntuple(a:Array F):% == 
        #a=n => per a
        error "wrong array length for ntuple"
    ntuple(t:Tuple F):% == 
        length t=n => per [t]
        error "wrong number of arguments for ntuple"
    val(t:%):Array F    ==  rep t
    dim(t:%):S          == #rep t 
    apply(t:%,i:S):F    ==  (rep t).i
    set!(t:%,i:S,x:F):% ==  {z:=rep t; z.i := x; per z}
    mag2(t:%):F ==
        x:F := 1
        for y in rep t repeat x := x * y
        x
    sum(t:%):F ==
        x:F := 0
        for y in rep t repeat x := x + y
        x
    (s:F)*(t:%):% == ntuple [s*rep(t).i for i:S in 1..dim t]
    0:% == ntuple new (n,0)
    1:% == ntuple new (n,1)
    (t1:%)+(t2:%):% == ntuple ([rep(t1).i+rep(t2).i for i:S in 1..n])
    (t1:%)*(t2:%):% == ntuple ([rep(t1).i*rep(t2).i for i:S in 1..n])
    (t1:%)-(t2:%):% == ntuple ([rep(t1).i-rep(t2).i for i:S in 1..n])
    -(t:%):%        == ntuple ([-rep(t).i for i:S in 1..n])
    (t:%)^(p:Integer):%   == 
        x:% := 1
        for i:S in 1..n repeat
            x := x * t
        x
    (t1:%)>(t2:%):Boolean == 
        q := true
        for i:S in 1..n repeat
            q := q and rep(t1).i>rep(t2).i
        q
    coerce(m:Integer):% == 
        t:% := 0
        for i:Integer in 1..m repeat
          t := t + 1
        t
    coerce(m:S):% ==
        t:% := 0
        for i:S in 1..m repeat
          t := t + 1
        t
    (t1:%)=(t2:%):Boolean ==
        q := true
        for i:S in 1..n repeat
            q := q and rep(t1).i=rep(t2).i
        q
    <<(t:TextWriter,nt:%):TextWriter == t << rep nt
    
NTupleProjection(F:FloatCategory,n1:S,n2:S): with 
    project: (NTuple(F,n1),Array S) -> NTuple(F,n2)
== add
    project(t1:NTuple(F,n1),ind:Array S):NTuple(F,n2) == 
        #ind = n2 => 
            a:Array F := [val(t1).(ind.k) for k in 1..n2]
            ntuple a
        error "wrong number of indices for projection in project"
     

S ==> SingleInteger
import from S

Lepton == 'electron, muon'

DataJet(F:FloatCategory): BasicType with
    datajet: (F,F,F)->(F,F,F)          ->  %
    datajet: (NTuple(F,3),NTuple(F,3)) ->  %
    distance2: (%,NTuple(F,3))         ->  F
    apply: (%,'px') -> F
    apply: (%,'py') -> F
    apply: (%,'pz') -> F
    apply: (%,'sigx') -> F
    apply: (%,'sigy') -> F
    apply: (%,'sigz') -> F
    apply: (%,S) -> (F,F)
    tag?:       % -> Boolean
    mutag?:     % -> Boolean
    etag?:      % -> Boolean
== add
    Rep ==> Record(p:NTuple(F,3),sigp:NTuple(F,3),tag:Boolean,leptype:Lepton)
    import from Lepton, Rep
    
    datajet(px:F,py:F,pz:F)(sx:F,sy:F,sz:F):% ==
        sx>=0.0 and sy>=0.0 and sz>= 0.0 => 
            per [ntuple(px,py,pz), ntuple(sx,sy,sz),false,electron]
        error "negative sigmas in datajet"
    datajet(t1:NTuple(F,3),t2:NTuple(F,3)):% == per [t1,t2,false,electron]
        
    =(a:%,b:%):Boolean == rep a = rep b
    sample: % == per [sample, sample, sample, sample ]
    apply(j:%,x:'px'):F == rep(j).p.1
    apply(j:%,x:'py'):F == rep(j).p.2
    apply(j:%,x:'pz'):F == rep(j).p.3
    apply(j:%,x:'sigx'):F == rep(j).sigp.1
    apply(j:%,x:'sigy'):F == rep(j).sigp.2
    apply(j:%,x:'sigz'):F == rep(j).sigp.3
    apply(j:%,i:S):(F,F) == 
        1<=i and i<=3 => (rep(j).p.i, rep(j).sigp.i)
        error "DataJet index out of range"
    tag?(j:%):Boolean == rep(j).tag
    mutag?(j:%):Boolean == rep(j).tag and rep(j).leptype=muon
    etag?(j:%):Boolean  == rep(j).tag and rep(j).leptype=electron
    <<(t:TextWriter,j:%):TextWriter == 
        for i in 1..3 repeat
            t << "Jet:"
            pr j.i where
                pr(x:F,y:F):() == t <<"    "<< x <<" +- " << y << newline
        t
    distance2(j:%,v:NTuple(F,3)):F == 1.0
