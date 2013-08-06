--* From youssef@mailer.scri.fsu.edu  Fri Aug  1 00:16:45 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA08376; Fri, 1 Aug 97 00:16:45 +0100
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id AAA06293 for <ax-bugs@nag.co.uk>; Fri, 1 Aug 1997 00:16:34 +0100 (BST)
--* Received: from sp2-4.scri.fsu.edu (sp2-4.scri.fsu.edu [144.174.128.94]) by mailer.scri.fsu.edu (8.8.5/8.7.5) with SMTP id TAA09289; Thu, 31 Jul 1997 19:14:51 -0400 (EDT)
--* From: Saul Youssef <youssef@scri.fsu.edu>
--* Received: by sp2-4.scri.fsu.edu (5.67b) id AA79855; Thu, 31 Jul 1997 19:14:46 -0400
--* Date: Thu, 31 Jul 1997 19:14:46 -0400
--* Message-Id: <199707312314.AA79855@sp2-4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs@nag.co.uk, edwards@scri.fsu.edu,
--*         youssef@scri.fsu.edu
--* Subject: [3] missing signature even though a domain has the right category

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -g interp
-- Version: 1.1.9a (AIX)
-- Original bug file name: bug0.as

--+ --
--+ -- Greetings;
--+ --
--+ --    It seems to me the the following shows a compiler bug.  If you 
--+ -- comment the last line, you will find that with % axiomxl -g interp
--+ -- (on AIX), the program runs and the domain "RunNumber" has 
--+ -- IntegerNumberSystem.  However, if you include the last line, the 
--+ -- compiler makes the following complaint:
--+ --
--+ --=====================================================================
--+ --"bug0.as", line 47: print << (x+y)@RunNumber << newline
--+ --                    ...........^
--+ --[L47 C12] #1 (Error) No one possible return type satisfies the context type.
--+ --  These possible return types were rejected:
--+ --          -- RunNumber
--+ --  The context requires an expression of type RunNumber.
--+ --The following could be suitable if imported:
--+ --  +: (RunNumber, RunNumber) -> RunNumber from RunNumber, if RunNumber has IntegerNumberSystem
--+ --========================================================================
--+ --
--+ -- ...even though RunNumber must have the requested signature since it 
--+ -- has IntegerNumberSystem and RunNumber has been explicitly imported.
--+ --
--+ --    Saul Youssef
--+ --    youssef@scri.fsu.edu
--+ --
--
-- Greetings;
--
--    It seems to me the the following shows a compiler bug.  If you 
-- comment the last line, you will find that with % axiomxl -g interp
-- (on AIX), the program runs and the domain "RunNumber" has 
-- IntegerNumberSystem.  However, if you include the last line, the 
-- compiler makes the following complaint:
--
--=====================================================================
--"bug0.as", line 47: print << (x+y)@RunNumber << newline
--                    ...........^
--[L47 C12] #1 (Error) No one possible return type satisfies the context type.
--  These possible return types were rejected:
--          -- RunNumber
--  The context requires an expression of type RunNumber.
--The following could be suitable if imported:
--  +: (RunNumber, RunNumber) -> RunNumber from RunNumber, if RunNumber has IntegerNumberSystem
--========================================================================
--
-- ...even though RunNumber must have the requested signature since it 
-- has IntegerNumberSystem and RunNumber has been explicitly imported.
--
--    Saul Youssef
--    youssef@scri.fsu.edu
--
#include "axllib"
#pile

SI ==> SingleInteger
import from SI

Name(C:Category,D:BasicType,n:String):C with 
  type:%->String
  coerce:D->%
  coerce:%->D
  <<:(TextWriter,%)->TextWriter 
== D add 
  Rep ==> D
  import from Rep
    
  type(x:%):String == n
  coerce(d:D):% == per d
  coerce(x:%):D == rep x
  <<(t:TextWriter,x:%):TextWriter == t << type(x) << ":" << rep x
    
RunNumber == Name(IntegerNumberSystem,SingleInteger,"Run Number")

import from RunNumber

if RunNumber has IntegerNumberSystem then print << "RunNumber has IntegerNumberSystem" << newline

x := 6::RunNumber
y := 7::RunNumber

print << x << newline
print << y << newline
--print << (x+y)@RunNumber << newline  -- uncomment this to see the error



