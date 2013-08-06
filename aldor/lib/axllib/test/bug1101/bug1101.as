--* Received: from server1.rz.uni-leipzig.de by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA03498; Wed, 18 Sep 96 12:40:07 BST
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.16/16.2) id AA055876426; Wed, 18 Sep 1996 13:33:46 +0200
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA46233; Wed, 18 Sep 1996 13:34:46 +0100
--* Date: Wed, 18 Sep 1996 13:34:46 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9609181234.AA46233@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs
--* Subject: [4] conditional exports

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl xxx.as
-- Version: AXIOM-XL version 1.1.6 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 18-SEP-96
-- AXIOM-XL version 1.1.6 for AIX RS/6000
-- Subject: conditional exports

-- When compiling the following lines with 
--   AXIOM-XL version 1.1.6 for AIX RS/6000 
-- via
--   axiomxl xxx.as
-- one get the following error message.

--:"xxx.as", line 26:       then g(DomX(E,T))
--:                   ....................^
--:[L26 C21] #1 (Error) Argument 2 of `DomX' did not match any possible parameter type.
--:    The rejected type is CatB(E).
--:    Expected type Join(CatA, CatB(E)).

--------------------- CUT HERE ---- file xxx.as ---------------------
#include "axllib"

Cat:Category == with { h:()->() }

CatA: Category == with {a:% -> ();}
CatB(E:CatA): Category == with {b:% -> ();}

DomX(E:CatA,T:Join(CatA,CatB E)):Cat == add {
  h():() == {import from String;
    print << "This is DomX." << newline;
  }
}
DomY(E:CatA,T:CatB E):Cat == add {
  h():() == {import from String;
    print << "This is DomY." << newline;
  }
}
GBP(E:CatA,T:CatB E):with {
  f: ()->();
  g: Cat->();
} == add {
  g(D:Cat):() == h()$D;
  f():() == {
    print << "T has AB: " << (T has Join(CatA,CatB E)) << newline;
    if T has Join(CatA,CatB E) 
      then g(DomX(E,T))
      else g(DomY(E,T));
  }
}
