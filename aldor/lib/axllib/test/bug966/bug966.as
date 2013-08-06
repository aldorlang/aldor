--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Fri May  5 05:16:06 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA21429; Fri, 5 May 1995 05:16:06 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0419; Fri, 05 May 95 05:16:05 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.0344.May.05.05:16:04.-0400>
--*           for asbugs@watson; Fri, 05 May 95 05:16:05 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Fri, 05 May 95 05:16:04 EDT
--* Received: from mendel.inf.ethz.ch (mendel.inf.ethz.ch [129.132.12.20]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id LAA10095 for <asbugs@watson.ibm.com>; Fri, 5 May 1995 11:15:40 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by mendel.inf.ethz.ch (8.6.10/8.6.10) id LAA18999 for asbugs@watson.ibm.com; Fri, 5 May 1995 11:11:16 +0200
--* Date: Fri, 5 May 1995 11:11:16 +0200
--* Message-Id: <199505050911.LAA18999@mendel.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [3] multiple meanings for same function [mean2.as][1.1]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

------------------------------- mean2.as ----------------------------------
--
-- % axiomxl -M2 mean2.as
-- "mean2.as", line 28:         ident(p:P):P == gcd(p,p);
--                      ........................^
-- [L16 C25] #1 (Error) There are 2 meanings for `gcd' in this context.
-- The possible types were:
--           gcd: (P, P) -> P from P
--           gcd: (P, P) -> P from P
--   The context requires an expression of type (P, P) -> P.
--

#include "axllib.as"

GcdDomain: Category == Ring with { gcd: (%, %) -> % };

Foo(R:Ring):Category == with {
	foo: % -> %;
	if R has GcdDomain then GcdDomain;
	if R has Field then {
-- COMPILES OK IF THE ORDER OF THOSE 2 LINES IS INVERTED!!!!
		EuclideanDomain;
		GcdDomain;
	}
}

Bar(F:Field, P:Foo F): with { ident: P -> P } == add {
	ident(p:P):P == gcd(p,p);
}
