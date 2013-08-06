--> testcomp
--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA03264; Thu, 22 Aug 96 10:41:02 BST
--* Received: from ru8.inf.ethz.ch (bronstei@ru8.inf.ethz.ch [129.132.12.17]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id LAA03982 for <ax-bugs@nag.co.uk>; Thu, 22 Aug 1996 11:34:55 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (from bronstei@localhost) by ru8.inf.ethz.ch (8.7.1/8.7.1) id LAA05947 for ax-bugs@nag.co.uk; Thu, 22 Aug 1996 11:34:53 +0200 (MET DST)
--* Date: Thu, 22 Aug 1996 11:34:53 +0200 (MET DST)
--* Message-Id: <199608220934.LAA05947@ru8.inf.ethz.ch>
--* To: ax-bugs
--* Subject: [1] inference failure with conditional exports

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.7
-- Original bug file name: badinfer.as

------------------------------- badinfer.as ----------------------------------
--
-- This is a result of fix966: the following does not compile any more.
-- This is a crucial bug, which kills lots of code using conditional exports,
-- and I don't have a workaround in sight.
--
-- % axiomxl -m2 badinfer.as
-- "badinfer.as", line 20:         ident(p:P):P == gcd(p, p);
--                         ........................^
-- [L26 C25] #1 (Error) There are no suitable meanings for the operator `gcd'.
-- The following could be suitable if imported:
--   gcd: (P, P) -> P from P, if R has GcdDomain
--

#include "axllib"

GcdDomain: Category == Ring with { gcd: (%, %) -> % };

Foo(R:Ring):Category == with {
	foo: % -> %;
	if R has GcdDomain then GcdDomain;
	if R has Field then GcdDomain;
}

Bar(R:Field, P:Foo R): with { ident: P -> P } == add {
	ident(p:P):P == gcd(p, p);
}
