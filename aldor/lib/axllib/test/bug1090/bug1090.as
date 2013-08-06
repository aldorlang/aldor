--* Received: from server1.rz.uni-leipzig.de by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA11109; Fri, 26 Jul 96 00:51:15 BST
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.16/16.2) id AA018398317; Fri, 26 Jul 1996 01:45:17 +0200
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA27602; Fri, 26 Jul 1996 01:45:28 +0100
--* Date: Fri, 26 Jul 1996 01:45:28 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9607260045.AA27602@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs
--* Subject: [2] Problem with 0$DirectProduct

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Q0 -Fao -V -Fo -Fx xxx.as
-- Version: AXIOM-XL version 1.1.6 for AIX RS/6000
-- Original bug file name: xxx.as

-- Compile with 
--  axiomxl -Q0 -Fao -V -Fo -Fx xxx.as
-- Running the programm xxx yields the following output

--:Robot
--:DirectProduct  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
--:The DP-ZERO.
--:Creation of E
--:ll =list(1, 1, 0, 0, 0)
--:CalixTerms !!!!!!!!!!!!!!!!!!!!!!!!!
--:This is DP
--:(0,0,0,0,0)Null
--:coerce DP
--:(0,0,0,0,0)
--:degreeList
--:Segmentation fault(coredump)

-- I hope that this program is short enough. I am unable to locate the 
-- error any further. My guess is that it has something to do with the
-- creation of 0 in DirectProduct or with 1 in CalixTerms. But I am not
-- really sure.

-- begin xxx.as --------------------------------------------------------

#include "axllib"

macro {
  B     == Boolean;
  SI    == SingleInteger;
  L     == List;
  DI    == DirectProduct(numOfVars,I);
  I     == SI;
  LN    == List I;
}
--------------------------------------------------------------------
define DirectProductCategory(S:AbelianMonoid): Category == AbelianMonoid
with {
  set!: (%,I,S) -> S;
  apply: (%, I) -> S;
  map: (S->S, %) -> %;
  map: ((S,S)->S, %,%) -> %;
  coerce: List S -> %;
  export from S;
}

DirectProduct(dim:I,S:AbelianMonoid): DirectProductCategory S ==
 PrimitiveArray S add {
  print << "DirectProduct  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << newline;
  Rep ==> PrimitiveArray S; 
  import from Rep; 
  inline from Rep;
  0: % == {print << "The DP-ZERO." << newline; per new(dim,0$S)};
  ZERO ==> 0;
  coerce(ls:List S):% == {
    w:Rep := new(dim,ZERO);
    for s in ls for i in 1..dim repeat w.i := s;
    per w
  }
  apply(x:%,i:I):S == (rep x).i;
  set!(x:%,i:I,s:S):S == set!(rep x,i,s);
  map(f:S-> S, v: %):% == {
    vv:Rep := new(dim,ZERO);
    for i in 1..dim repeat vv.i := f((rep v).i);
    per vv;
  }
  map(f:(S,S) -> S, v1:%, v2:%): % == {
    vv:Rep := new(dim,ZERO);
    for i in 1..dim repeat vv.i := f((rep v1).i,(rep v2).i);
    per vv
  }
  (v1: %) = (v2: %): B == {
    for i in 1..dim repeat (rep v1).i ~= (rep v2).i => return false;
    true
  }
  (p: TextWriter) << (v: %): TextWriter == {
    print << "DP" << newline;
    dim=0 => p << "()";
    p << "(" << (rep v).1;
    for i in 2..dim repeat p  << "," << (rep v).i;
    p << ")"
  }
  zero?(v:%):B == v = 0;
  (v1: %) + (v2: %): % == map(+$S, v1, v2);
  +(v: %): % == v;
}

CalixTerms(
    numOfVars: I,
    weightList: L DI
  ): Join(Order, Monoid) with {
  coerce: LN -> %;
} == add {
  print << "CalixTerms !!!!!!!!!!!!!!!!!!!!!!!!!" << newline;
  macro {
    EX(x)   == (rep(x).ex);
    LDEG(x) == (rep(x).ldeg);
  }
  Rep ==> Record(ex:DI,ldeg:L I);
  import from Rep;
  1:% == {import from L I; print << "This is " << (0$DI)<< "Null" << newline; 
  --  per [0$DI, [0$I for i in weightList]]}; 
    0$DI::%}
  sample:% == 1$%;
  local dot(di:DI,dn:DI):I == {
    print << "DOT " << di << " and " << dn << newline;
    s:I := di.1 * dn.1;
    for i:I in 2..numOfVars repeat { s := s + di.i * dn.i }
    s;
  }
  local degreeList(z:DI):L(I) == {print << "degreeList" << newline;
    [dot(weight,z) for weight in weightList];}
  coerce(dn:DI):% == {print << "coerce " << dn << newline;
    x:L(I) := degreeList dn;
    -- [dn, degreeList dn]};
    per [dn, x]};
  coerce(ln:LN):% == {
    print << "COERCE-LN: " << ln << newline;
    if #ln = numOfVars then {
      ln::DI::%
    }else{
      error "length of given list does not match number of variables"
    }
  }
  (x:%) = (y:%):B == EX x = EX y;
  (x:%) ^ (n:Integer):% == power(1,x,n)$BinaryPowering(%,*,Integer);
  (p:TextWriter) << (x:%):TextWriter == {
    vars:L String:= ["S1","S2","S3","L2","L3"];
    zero?(EX x) => p << 1$SI;
    printed: B := false;
    for i:SI in 1..numOfVars|not zero?(e:=EX(x).i) repeat {
      if printed then p << "*";
      p << vars.i; printed := true;
      if e~=1 then p << "^" << e;
    }
    p;
  }
  (x:%) * (y:%):% == per [EX x+EX y, [i+j for i in LDEG x for j in LDEG y]];
  (x:%) > (y:%):B == true;
}
----------------------------------------------------------------------------
MAIN():() == {
print << "Robot" << newline;
import from List List I;
numOfVars:I == 5;
weightList:List List I := [[1,1,1,0,0], [0,0,0,1,1]];
wl:List(DI) == [w::DI for w in weightList];
print << "Creation of E" << newline;
E == CalixTerms(numOfVars,wl);
import from E,I;
print << "ll =";
ll:LN := [1,1,0,0,0];
print <<ll<< newline;
print << (ll::E) << " = 1$E" << newline
}
MAIN();
