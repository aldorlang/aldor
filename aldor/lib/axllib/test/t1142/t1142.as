--> testgen c -O 

-- This works only without `-Wcheck'.  This is because the
-- real fix is to get the inliner right,
-- not hacking genc.

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
