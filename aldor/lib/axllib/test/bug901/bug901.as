--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Nov  4 15:59:44 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA20511; Fri, 4 Nov 1994 15:59:44 -0500
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2587; Fri, 04 Nov 94 15:59:50 EST
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.HEMMECKE.NOTE.YKTVMV.1083.Nov.04.15:59:49.-0500>
--*           for asbugs@watson; Fri, 04 Nov 94 15:59:49 -0500
--* Received: from server1.rz.uni-leipzig.de by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Fri, 04 Nov 94 15:59:47 EST
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.38.193.5/15.6) id AA02010; Fri, 4 Nov 1994 21:59:34 +0100
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA38898; Fri, 4 Nov 1994 21:59:14 +0100
--* Date: Fri, 4 Nov 1994 21:59:14 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9411042059.AA38898@aix550.informatik.uni-leipzig.de>
--* To: asbugs@watson.ibm.com
--* Subject: [3] identifying `%' with a certain domain

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: asharp -DC2 -DCA xxx.as
-- Version: A# version 0.36.5 for AIX RS/6000
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 04-NOV-94
-- A# version 0.36.5 for AIX RS/6000

-- S. Dooley once told me that in general `%' in a category definition
-- refers to any domain in this category and thus it cannot simply identified
-- with a particular domain. However, inside an add expression A# could do
-- this identification (in our case below it is `%' and `Dom').

-- Unfortunately, this is not always true.
-- We have 6 ways to compile the following code.

-- asharp -DC1 -DCA xxx.as
-- asharp -DC1 -DCA -DCB xxx.as
-- asharp -DC2 -DCA -DCB xxx.as
-- Compiling with the options above there will be no messages.

-- asharp -DC1 -DCB xxx.as
-- gives:
--:"xxx.as", line 6:   Dom:Join(Cat1,Cat2(Dom)) with == Integer add {
--:                  ...........................................^
--:[L6 C44] #1 (Error) The domain is missing some exports.
--:        Missing f: % -> %
-- This seems to be understandable.

-- asharp -DC2 -DCA xxx.as
-- gives:
--:"xxx.as", line 17:   Dom:Join(Cat1,Cat2(Dom)) with == Integer add {
--:                   ...........................................^
--:[L17 C44] #1 (Error) The domain is missing some exports.
--:        Missing f: % -> Partial(Dom)

-- asharp -DC2 -DCB xxx.as
-- gives:
--:"xxx.as", line 17:   Dom:Join(Cat1,Cat2(Dom)) with == Integer add {
--:                   ...........................................^
--:[L17 C44] #1 (Error) The domain is missing some exports.
--:        Missing f: % -> Partial(%)
-- This seems to be understandable as well.

-- Instead of the function f one could imagine to have multiplication in
-- Cat1 (which could be a monoid) and a left action of the domain D over
-- a domain from Cat2 D. The domain Dom wants to bring together the left
-- action and the monoid multiplication to have only one single funtion.

---------------------------------------------------------------------
#include "axllib"
macro BT == BasicType;
#if C1
  Cat1: Category         == BT with { f: % -> % }
  Cat2(D:Cat1): Category == BT with { f: % -> D }
  Dom:Join(Cat1,Cat2(Dom)) with == Integer add {
#if CA
    f(d:%):% == d;
#endif
#if CB
    f(d:%):Dom == d;
#endif
}
#elseif C2
  Cat1: Category         == BT with { f: % -> Partial % }
  Cat2(D:Cat1): Category == BT with { f: % -> Partial D }
  Dom:Join(Cat1,Cat2(Dom)) with == Integer add {
#if CA
    f(d:%):Partial(%) == d::Partial(%);
#endif
#if CB
    f(d:%):Partial(Dom) == d::Partial(Dom);
#endif
  }
#endif
