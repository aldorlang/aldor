--* Received: from server1.rz.uni-leipzig.de by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA01539; Thu, 21 Mar 96 14:28:42 GMT
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.16/16.2) id AA270628220; Thu, 21 Mar 1996 15:23:41 +0100
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA11818; Thu, 21 Mar 1996 15:26:03 +0100
--* Date: Thu, 21 Mar 1996 15:26:03 +0100
--* From: hemmecke@de.uni-leipzig.informatik.aix550 (Ralf Hemmecke)
--* Message-Id: <9603211426.AA11818@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@uk.co.nag
--* Subject: [8] assignments in macros

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -DC2 -Fx xxx.as
-- Version: AXIOM-XL version 1.1.4 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 21-MAR-96
-- AXIOM-XL version 1.1.4 for AIX RS/6000
-- Subject: assignments in macros

-- This piece of code seems due to a lack of information how
-- macro expansion really works.

-- Compile with
--   axiomxl -DC1 -Fx xxx.as
-- or use
--   axiomxl -DC2 -Fx xxx.as

-- The only difference between the two options is that C1 does not
-- use the macro commonFunctions but puts everything directly into
-- the add part whereas C2 first defines the macro commonFunctions
-- for later use within the add part.

-- Background is that there is actually another constructor CalixTerms
-- which does nearly the same thing but has a different parameter list 
-- and also uses a different representation. I did not want to write 
-- the code twice but a package instead of a macro seemed impossible
-- because of the (domain) variable varTable.

-- The option C1 works just fine and produces:

--: Main initializes ...
--: The variables: CalixTerms
--: BEGIN init
--: var: 1
--: var: 2
--: var: 3
--: END init
--: list(x, y, z)

-- The option C2 gives:

--: Main initializes ...
--: The variables: CalixTerms
--: END init
--: var: 1
--: Illegal instruction(coredump)

-- If this behaviour is not a bug, would you please explain the
-- reasons.
-----------------------------------------------------------------------
#include "axllib"
macro {
  SI == SingleInteger;
  BT == BasicType;
  CT == BT with { variables:List String };
}
import from List String;
-----------------------------------------------------------------------
#if C1
CalixTerms(numOfVars: SI, vars: List String): CT == add {
  print << "CalixTerms" << newline;
  Rep ==> Integer; import from Rep;
  (x:%) = (y:%):Boolean == true;
#else
macro commonFunctions == {
#endif
  sample:% == per 1;
  variables:List String == vars;
  (p:TextWriter) << (x:%):TextWriter == p << rep(x);
  varTable:HashTable(String,SI) := table();
  print << "BEGIN init" << newline;
  for i in 1..numOfVars repeat {
    print << "var: " << i << newline; varTable(vars.i) := i;}
  print << "END init" << newline;
}
----------------------------------------------------------------
#if C1
#else
CalixTerms(numOfVars: SI, vars: List String): CT == add {
  print << "CalixTerms" << newline;
  Rep ==> Integer; import from Rep;
  (x:%) = (y:%):Boolean == true;
  commonFunctions
}
#endif
---------------------------------------------------------------------
MAIN():() == {
print << "Main initializes ..." << newline;
vars == ["x","y","z"];
numOfVars:SI == #vars;
E:CT == CalixTerms(numOfVars,vars);
print << "The variables: " << variables$E << newline;
}
MAIN();
