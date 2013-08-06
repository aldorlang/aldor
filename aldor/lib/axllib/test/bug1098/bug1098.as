--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA16794; Fri, 6 Sep 96 14:31:52 BST
--* Received: from ru8.inf.ethz.ch (bronstei@ru8.inf.ethz.ch [129.132.12.17]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id PAA02851 for <ax-bugs@nag.co.uk>; Fri, 6 Sep 1996 15:25:37 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (from bronstei@localhost) by ru8.inf.ethz.ch (8.7.1/8.7.1) id PAA29061 for ax-bugs@nag.co.uk; Fri, 6 Sep 1996 15:25:35 +0200 (MET DST)
--* Date: Fri, 6 Sep 1996 15:25:35 +0200 (MET DST)
--* Message-Id: <199609061325.PAA29061@ru8.inf.ethz.ch>
--* To: ax-bugs
--* Subject: [1] conditional export problem

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.7a
-- Original bug file name: nobar.as

----------------------------- nobar.as ----------------------------------
--
-- Finally, here is *the* bug that causes an export not be seen when
-- needed:
--
-- % axiomxl -m2 nobar.as
-- "nobar.as", line 42: macro FOO0 == Foo(R, %);
--                      ..................^
-- [L42 C19] #1 (Error) (After Macro Expansion) Argument 1 of `Foo' did not match any possible parameter type.
--     The rejected type is Ring.
--     Expected type EuclideanDomain.
-- Expanded expression was: R
--  
-- "nobar.as", line 50:                                 c0 := bar(p, q)$FOO0;
--                      ...............................................^
-- [L50 C48] #2 (Error) There are no suitable meanings for the operator `bar$Foo(R, %)'.
--  
-- "nobar.as", line 51:                                 c1 := bar(p, q)$FOO1;
--                      ...............................................^
-- [L51 C48] #3 (Error) There are no suitable meanings for the operator `bar$Foo(R pretend EuclideanDomain, %)'.
--  
-- "nobar.as", line 52:                                 c2 := bar(p, q)$FOO2;
--                      ...............................................^
-- [L52 C48] #4 (Error) There are no suitable meanings for the operator `bar$Foo(R pretend Field, %)'.
-- 

#include "axllib"

MyCategory0(R:Ring):Category == with {
	baz: % -> %;
	if R has Field then Field;
}

Foo(R:EuclideanDomain, P:MyCategory0 R): with {
	foo: P -> P;
	if R has Field then bar: (P, P) -> P;
} == add {
	foo(a:P):P == a;
	if R has Field then bar(a:P, b:P):P == a;
}

macro FOO0 == Foo(R, %);
macro FOO1 == Foo(R pretend EuclideanDomain, %);
macro FOO2 == Foo(R pretend Field, %);

MyCategory(R:Ring):Category == MyCategory0 R with {
	default {
		if R has Field then {
			(p:%) / (q:%):%	== {
				c0 := bar(p, q)$FOO0;
				c1 := bar(p, q)$FOO1;
				c2 := bar(p, q)$FOO2;
				p;
			};
		}
	}
}
