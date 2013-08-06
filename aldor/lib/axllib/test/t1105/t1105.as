--> testcomp
--* From hemmecke@aix550.informatik.uni-leipzig.de  Tue Nov  5 13:51:20 1996
--* Received: from nags2.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA09710; Tue, 5 Nov 96 13:51:20 GMT
--* Received: from server1.rz.uni-leipzig.de by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA16961; Tue, 5 Nov 96 13:57:30 GMT
--* Received: from aix550.informatik.uni-leipzig.de by server1.rz.uni-leipzig.de with SMTP
--* 	(1.37.109.16/16.2) id AA274000858; Tue, 5 Nov 1996 14:34:19 +0100
--* Received: by aix550.informatik.uni-leipzig.de (AIX 3.2/UCB 5.64/BelWue-1.1AIXRS)
--*           id AA49986; Tue, 5 Nov 1996 14:30:59 +0100
--* Date: Tue, 5 Nov 1996 14:30:59 +0100
--* From: hemmecke@aix550.informatik.uni-leipzig.de (Ralf Hemmecke)
--* Message-Id: <9611051330.AA49986@aix550.informatik.uni-leipzig.de>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] Type recognition

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Mno-abbrev xxx.as
-- Version: AXIOM-XL version 1.1.7 for LINUX
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 05-NOV-96
-- AXIOM-XL version 1.1.7 for LINUX 
-- Subject: Type recognition

-- The following strange compiler message appears with appears only
-- with the Linux version of A#. There are no problems with
-- AXIOM-XL version 1.1.6 for AIX RS/6000.

--: "xxx.as", line 8: DOM(T: with {weight: % -> SingleInteger},
--:                   ....^
--:[L8 C5] #1 (Error) Have determined 1 possible types for the expression.
--:        Meaning 1: (T:  with weight: % -> SingleInteger, M:  with leadingTerm: % -> T, C: CAT(M) with lcm: % -> T) -> CAT(M)
--:  The context requires an expression of type (T:  with weight: % -> SingleInteger, M:  with leadingTerm: % -> T, C: CAT(M) with lcm: % -> T) -> CAT(M).

---------------------- xxx.as ----------------------------
#include "axllib"

define CAT(M:Type):Category == with {f: M -> M}

DOM(T: with {weight: % -> SingleInteger},
  M: with {leadingTerm: % -> T},
  C: CAT M with {lcm: % -> T}
): CAT M == SingleInteger add {
  f(m:M):M == m;
}

