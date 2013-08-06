-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Sun May 14 07:42:01 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA20437; Sun, 14 May 1995 07:42:01 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2357; Sun, 14 May 95 07:41:59 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BRONSTEI.NOTE.YKTVMV.9934.May.14.07:41:58.-0400>
--*           for asbugs@watson; Sun, 14 May 95 07:41:58 -0400
--* Received: from inf.ethz.ch by watson.ibm.com (IBM VM SMTP V2R3) with TCP;
--*    Sun, 14 May 95 07:41:57 EDT
--* Received: from mendel.inf.ethz.ch (mendel.inf.ethz.ch [129.132.12.20]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id NAA14436 for <asbugs@watson.ibm.com>; Sun, 14 May 1995 13:41:56 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by mendel.inf.ethz.ch (8.6.10/8.6.10) id NAA22193 for asbugs@watson.ibm.com; Sun, 14 May 1995 13:37:18 +0200
--* Date: Sun, 14 May 1995 13:37:18 +0200
--* Message-Id: <199505141137.NAA22193@mendel.inf.ethz.ch>
--* To: asbugs@watson.ibm.com
--* Subject: [1] Looks like bad symbols exported from .ao [buggy.as][1.1.0 (latest)]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-------------------------------- buggy.as ---------------------------------
-- TO REPRODUCE THIS BUG, YOU MUST FIRST BREAK THIS INTO THE 3 SEPARATE
-- FILES  tree.as  tools.as  minus.as
-- IT IS ALSO NECESSARY TO ARCHIVE tree.ao INTO A NEW LIBRARY tree.al
-- WITH ar, OTHERWISE THINGS COMPILE OK
-- THIS WORKAROUND IS UNUSABLE IN PRACTICE!
--
-- % axiomxl tree.as
-- % ar rv tree.al tree.ao
-- a - tree.ao
-- ar: creating tree.al
-- % axiomxl tools.as minus.as
--
-- tools.as:
--
-- minus.as:
-- "minus.as", line 14:
--         tex(p:TEXT, l:List Tree):TEXT   == prefix(p, first l);
-- .....................................................^
-- [L14 C54] #1 (Error) Argument 2 of `prefix' did not match any possible parameter type.
--     The rejected type is Tree.
--     Expected one of:
--       -- Tree
--       -- List(Tree)
--

------------------------------- tree.as ----------------------------------
#include "axllib.as"

macro {
	Z	== SingleInteger;
	TEXT	== TextWriter;
}

Operator: Category == with { tex: (TEXT, List Tree) -> TEXT };

Leaf: BasicType with {
	leaf:		SingleInteger -> %;
	tex:		(TEXT, %) -> TEXT;
} == add {
	macro Rep == Z;

	import from Rep;

	sample:%			== leaf(1@Z);
	leaf(n:SingleInteger):%		== per n;
	(p:TEXT) << (l:%):TEXT		== p;
	tex(p:TEXT, l:%):TEXT		== p;
	(a:%) = (b:%):Boolean		== false;
}

Tree: BasicType with {
	arguments:	% -> List %;
	extree:		Z -> %;
	leaf:		% -> Leaf;
	leaf?:		% -> Boolean;
	operator:	% -> Operator;
	tex:		(TEXT, %) -> TEXT;
} == add {
	macro {
		XTree == Record(oper:Operator, argum: List %);
		Rep == Union(uleaf: Leaf, utree: XTree);
	}

	import from Rep;

	sample:%			== { import from Z; extree 1; }
	tree(l:Leaf):%			== per [l];
	tree(r:XTree):%			== per [r];
	extree(n:Z):%			== tree leaf n;
	apply(op:Operator, l:List %):%	== tree [op, l];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	tree(t:%):XTree			== rep(t).utree;
	leaf(t:%):Leaf			== rep(t).uleaf;
	operator(t:%):Operator		== operator tree t;
	arguments(t:%):List %		== arguments tree t;
	operator(t:XTree):Operator	== t.oper;
	arguments(t:XTree):List %	== t.argum;
	tex(p:TEXT, t:%):TEXT		== switchon(p, t, tex, tex);
	(p:TEXT) << (t:%):TEXT		== p;
	(x:%) = (y:%):Boolean		== false;

	switchon(p:TEXT, t:%, f:(TEXT, Leaf) -> TEXT,
		g:(TEXT, XTree) -> TEXT):TEXT == {
			leaf? t => f(p, leaf t);
			g(p, tree t);
	}

	tex(p:TEXT, t:XTree):TEXT ==
		tex(p, arguments(t) pretend List Tree)$operator(t);
}

-------------------------- tools.as --------------------------
#include "axllib.as"

#library libtree "tree.al"
import from libtree;

macro TEXT == TextWriter;

Tools: with {
	prefix: (TEXT, Tree) -> TEXT;
	prefix: (TEXT, List Tree) -> TEXT;
} == add {
	import from SingleInteger;

	prefix(p:TEXT, t:Tree):TEXT == p;
	prefix(p:TEXT, t:List Tree):TEXT == p;
}

-------------------------- minus.as --------------------------
#include "axllib.as"

-- COMPILES OK IF tree.al IS REPLACED BY tree.ao
#library libtree "tree.al"
#library libtools "tools.ao"
import from libtree, libtools;

macro TEXT == TextWriter;

Minus:Operator == add {
	import from Tools;

	tex(p:TEXT, l:List Tree):TEXT   == prefix(p, first l);
}
