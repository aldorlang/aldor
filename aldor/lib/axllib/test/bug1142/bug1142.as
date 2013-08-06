--* From Manuel.Bronstein@sophia.inria.fr  Wed Jun  3 17:43:28 1998
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA10581; Wed, 3 Jun 98 17:43:28 +0100
--* Received: from nirvana.inria.fr (bmanuel@nirvana.inria.fr [138.96.48.30])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id RAA01428 for <ax-bugs@nag.co.uk>; Wed, 3 Jun 1998 17:45:09 +0100 (BST)
--* Received: by nirvana.inria.fr (8.8.8/8.8.5) id SAA01416 for ax-bugs@nag.co.uk; Wed, 3 Jun 1998 18:42:30 +0200
--* Date: Wed, 3 Jun 1998 18:42:30 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <199806031642.SAA01416@nirvana.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [1] C generation broken in 1.11c

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fo bug11c.as
-- Version: 1.1.11c
-- Original bug file name: bug11c.as

------------------------------- bug11c.as ----------------------------------
--
-- It looks like the C generation and optimization is broken in 1.11c:
--
-- % axiomxl -fo -q1 bug11c.as
-- cc: Warning: bug11c.c, line 557: In this statement, "CF14_tree(...)" of type
-- "pointer to struct Fmt9", is being converted to "unsigned long".
--         T0 = CF14_tree(e1, l0->X0_t);
-- --------^
-- cc: Warning: bug11c.c, line 600: In this statement, "CF14_tree(...)" of type
-- "pointer to struct Fmt9", is being converted to "unsigned long".
--         T1 = CF14_tree(e1, P1_t);
-- --------^
--
-- % axiomxl -fo -q2 bug11c.as
-- cc: Error: bug11c.c, line 483: Missing ";".
--         PFmt4 T0;
-- --------------^
-- cc: Error: bug11c.c, line 486: In this statement, "T0" is not declared.
-- L0:     T0 = (FiWord) ((PFmt8) P0_t)->X1_value;
-- --------^
-- #1 (Fatal Error) C compile failed.  Command was:
--   unicl -O -I/net/safir/lib/axiomxl-1.1.11c/decosf1/include -c bug11c.c
--

#include "axllib"

macro {
	Z	== SingleInteger;
	OP	== ExpressionTreeOperator;
	TEXT	== TextWriter;
	TREE	== ExpressionTree;
}

ExpressionTreeOperator: Category == with { uniqueId: Z; }

ExpressionTreeLeaf: BasicType with { leaf: Integer -> %; } == add {
	macro Rep == Integer;
	import from Rep;
	sample:%			== leaf(1@Integer);
	leaf(n:Integer):%		== per n;
	(p:TEXT) << (l:%):TEXT		== p;
	(a:%) = (b:%):Boolean		== false;
}

ExpressionTree: BasicType with {
	leaf:		% -> ExpressionTreeLeaf;
	leaf?:		% -> Boolean;
	operator:	% -> OP;
} == add {
	macro {
		Tree == Record(oper:OP, argum: List %);
		Leaf == ExpressionTreeLeaf;
		Rep == Union(uleaf: Leaf, utree: Tree);
	}

	import from Rep;

	sample:%			== per [sample$Leaf];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	local tree(t:%):Tree		== rep(t).utree;
	leaf(t:%):Leaf			== rep(t).uleaf;
	operator(t:%):OP		== operator tree t;
	local operator(t:Tree):OP	== t.oper;
	(x:%) = (y:%):Boolean		== false;

	(p:TEXT) << (t:%):TEXT == {
		leaf? t => p << leaf t;
		stream(p, tree t);
	}

	local stream(p:TEXT, t:Tree):TEXT == p;
}
