-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint
--> testrun -l axllib
#include "axllib.as"
#pile

Tree(S: BasicType): BasicType with {
	empty: %;
	tree:  S  -> %;
	tree:  (S, %, %) -> %;

	empty?: % -> Boolean;

	left:   % -> %;
	right:  % -> %;
	node:   % -> S;

	preorder:  % -> Generator S;
	inorder:   % -> Generator S;
	postorder: % -> Generator S;
} == {
	if S has Ring then error "Use a different type...";
	add {
	Rep ==> Record(node: S, left: %, right: %);
	import from Rep;

	empty: % == nil$Pointer pretend %;
	empty?(t: %): Boolean == nil?(t pretend Pointer)$Pointer;

	tree(s: S): % == per [s, empty, empty];
	tree(s: S, l: %, r: %): % == per [s, l, r];

	local nonempty(t: %): Rep == {
		empty? t => error "Taking a part of a non-empty tree";
		rep t
	}

	left (t: %): % == nonempty(t).left;
	right(t: %): % == nonempty(t).right;
	node (t: %): S == nonempty(t).node;
	
	preorder(t: %): Generator S == generate {
		if not empty? t then {
			yield node t;
			for n in preorder left  t repeat yield n;
			for n in preorder right t repeat yield n;
		}
	}
	
	inorder(t: %): Generator S == generate {
		if not empty? t then {
			for n in inorder left  t repeat yield n;
			yield node t;
			for n in inorder right t repeat yield n;
		}
	}
	
	postorder(t: %): Generator S == generate {
		if not empty? t then {
			for n in postorder left  t repeat yield n;
			for n in postorder right t repeat yield n;
			yield node t;
		}
	}

	(outp: TextWriter) << (t: %): TextWriter == {
		import from String;
		import from S;
		empty? t => outp << "empty";
		empty? left t and empty? right t => outp<<"tree "<<node t;
		outp<<"tree("<<node t<<", "<<left t<<", "<<right t<<")"
	}
	sample: % == empty;
	(t1: %) = (t2: %): Boolean == {
		import from S;
		empty? t1 and  empty? t2 => true;
		empty? t1 or   empty? t2 => false;
		node t1 = node t2 and left t1 = left t2 and right t1 = right t2
	}
}
}

f(): () == {
	import from String;
	import from Tree(String);
	import from TextWriter;

	t := tree("*", tree("1", tree "a", tree "b"),
		       tree("2", tree "c", tree "d"));

	
	print<<"The tree is "<<t<<newline;

	print<<"Preorder: ";
	for s in preorder t repeat print<<s<<" ";
	print<<newline;

	print<<"Inorder: ";
	for s in inorder t repeat print<<s<<" ";
	print<<newline;

	print<<"Postorder: ";
	for s in postorder t repeat print<<s<<" ";
	print<<newline;
}

f()
