--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Feb 14 07:24:59 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA28533; Tue, 14 Feb 1995 07:24:59 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 9985; Tue, 14 Feb 95 07:24:33 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.PETERB.NOTE.YKTVMV.5215.Feb.14.07:24:33.-0500>
--*           for asbugs@watson; Tue, 14 Feb 95 07:24:33 -0500
--* Received: from sun2.nsfnet-relay.ac.uk by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Tue, 14 Feb 95 07:24:27 EST
--* Via: uk.co.iec; Tue, 14 Feb 1995 11:37:35 +0000
--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1) id AA19133;
--*           Tue, 14 Feb 95 11:39:15 GMT
--* From: Peter Broadbery <peterb@num-alg-grp.co.uk>
--* Date: Tue, 14 Feb 95 11:36:22 GMT
--* Message-Id: <17299.9502141136@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/NAg-1.0) id AA17299;
--*           Tue, 14 Feb 95 11:36:22 GMT
--* To: asbugs@watson.ibm.com
--* Subject: forwarded bug from hemmecke@aix550.informatik.uni-leipzig.de

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: axiomxl -DC1 -gloop
-- Version: AXIOM-XL version 1.0.0 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 08-FEB-95
-- AXIOM-XL version 1.0.0 for AIX RS/6000

-- This is a simplified part of code I want to have for Groebner bases
-- calculations.

-- Use it the following way:
--      axiomxl -DC1 -gloop
--      #include "xxx.as"

-- or with an option C2
--      axiomxl -DC2 -gloop
--      #include "xxx.as"

-- The only difference is the appearance of a redefinition of primitivePart
-- in CompoundPoly.
-- As the output shows, primitivePart$CompoundPoly uses the content
-- function defined in the default part of the category Cat.
-- Whereas C2 uses the (wanted) definition of content from CompoundPoly.

-- Thinking of an object oriented approach, one would think that C1 should
-- be enough to have what one wants.
-- I came across this strange behaviour of axiom-xl because of a
-- runtime-error where I never expected one.
-- Thus, when one wants to redefine a function one always has to check
-- whether or not there is a (default) function which does not the thing one
-- would expect. I think this is a source of hard detectable errors.

#include "axllib.as"
B ==> Boolean;
I ==> Integer;
LI ==> List I;
TW ==> TextWriter;
Cat:Category == with {_
  coerce: LI -> %;
  zero?: % -> B;
  leadingCoefficient: % -> I;
  reductum: % -> %;
  <<: (TW,%) -> TW;
  quo: (%,I) -> %;
  content: % -> I;
  primitivePart: % -> %;
  default{
    content(x:%):I == {
      zero? x => 0;
      r:=leadingCoefficient x;
      while not zero?(x:=reductum x) and r~=1 repeat
        r:=gcd(r,leadingCoefficient x);
      r;
    }
    primitivePart(x:%):% == { zero? x => x; x quo content(x); }
  }
};

Poly:Cat == List I add {
  Rep ==> List I;
  import from Rep;
  X ==> rep x;
  coerce(li:LI):% == per li;
  zero?(x:%):B == empty? X;
  leadingCoefficient(x:%):I == first X;
  reductum(x:%):% == per rest X;
  (p:TW) << (x:%):TW == p << X;
  (x:%) quo (i:I):% == per [e quo i for e in X];
}

CompoundPoly(P:Cat,Q:Cat):Cat with { convert: (P,Q) -> % } == add {
  Rep ==> Record(p:P,q:Q);
  import from Rep;
  X ==> rep x;
  coerce(li:LI):% == convert(li::P,empty()$LI::Q);
  convert(pp:P,qq:Q):% == per [pp,qq];
  zero?(x:%):B == zero? X.p;
  leadingCoefficient(x:%):I == leadingCoefficient X.p;
  reductum(x:%):% == convert(reductum X.p, X.q);
  (pr:TW)<<(x:%):TW == pr << "(" << (X.p) << "<->" << (X.q) << ")";
  (x:%) quo (i:I):% == convert(X.p quo i,X.q quo i);
  content(x:%):I == gcd(content(X.p),content(X.q));
#if C1
#elseif C2
  primitivePart(x:%):% == { zero? x => x; x quo content(x); }
#endif
}

import from LI;
p1:= [8,4,4]::Poly;
p2:= [6,3]::Poly;
CP == CompoundPoly(Poly,Poly);
import from CP;
c := convert(p1,p2);
print << "c = " << c << newline;
print << "content:    " << content(p1) << " <-> " << content(p2) << newline;
print << "content c = " << content(c) << newline;
print << "primitivePart c = " << primitivePart(c) << newline;




