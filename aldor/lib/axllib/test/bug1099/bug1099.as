--* Received: from red.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA00918; Fri, 13 Sep 96 14:19:42 BST
--* Received: from frisco.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for ax-bugs@nag.co.uk id AA08866; Fri, 13 Sep 96 14:12:45 +0100
--* From: peterb@red.nag.co.uk (Peter Broadbery)
--* Date: Fri, 13 Sep 1996 14:12:53 +0100
--* Message-Id: <199609131312.OAA02313@frisco>
--* Received: by frisco (SMI-8.6) id OAA02313; Fri, 13 Sep 1996 14:12:53 +0100
--* To: ax-bugs%nag.co.uk@red.nag.co.uk
--* Subject: [2] Substitution ****s up

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -ginterp x2.as
-- Version: 1.1.7
-- Original bug file name: x2.as

#include "axllib"

-- the problem is the 'S: BasicType'.  Renaming them to R,S and T
-- makes the problem go away.  Looking deeper, this is because the
-- type on 'step!' in BasicHomogeneousAggregate contains 'S' from 
-- BasicAggregate.  This means that the computed hashcode is a load 
-- of old cobblers, and so the lookup fails.

NNI	==>NonNegativeInteger;
BSI	==>BasicSimpleIterations;
SI	==>SingleInteger; 



+++	BasicAggregate serves to model any data structure 
+++	aggregate, designating any collection of objects, with heterogenous 
+++	or homogeneous members, with a finite or infinite number of members, 
+++	explicitly or implicitly represented. An aggregate can in principle
+++	represent everything from a string of characters to abstract sets such
+++	as "the set of x satisfying relation r(x)". An attribute finiteAggregate 
+++	is used to assert that a domain element contains a finite number of 
+++	objects.
+++	Date Created: 1995
+++	Keywords: type, aggregate, finite
define BasicAggregate(S: BasicType): Category == Conditional with {
	export from S;
	eq?:		(%, %) -> Boolean;
		++ eq?(u,v) tests if u and v are the same object.
	copy:		% -> %;
		++ copy(u) returns a top-level (non recursive) copy of u.
	empty:		() -> %;
		++ empty()$D creates an aggregate of type D with no elements.
		++ Axioms: # empty() = 0, empty? empty() = true.
		++ Note: empty() is allowed to produce an error if the domain 
		++ does not support the empty aggregate.
}

+++	BasicHomogeneousAggregate is an aggregate of elements all of the
+++	same type. In the current system, all aggregates are homogeneous.
+++	Two attributes characterize classes of aggregates.
+++	Aggregates from domains with attribute finiteAggregate have a 
+++	finite number of members. Those with attribute shallowlyMutable 
+++	allow an element to be modified or updated without changing its overall 
+++	value.
define BasicHomogeneousAggregate(S:BasicType): Category == 
	BasicAggregate(S) with {
	generator: 	% -> Generator S;
		++ Generic traversal of a homogeneous aggregate.
default {
	(x:%) = (y:%):Boolean == {
		-- use the eq? test
		eq?(x,y) => true;
		-- use the values from the Generator 
		import from Generator S;
		gx:=generator x;
		gy:=generator y;
		repeat { 
			step! gx;
			step! gy;
			(empty? gx and empty? gy) => return true;
			(empty? gx and not empty? gy) or
				(empty? gy and not empty? gx) => return false;
			value gx ~= value gy => return false;
			}	
		}
	}
}




+++	A bag aggregate is an aggregate for which one can insert and extract 
+++	objects, and where the order in which objects are inserted determines 
+++	the order of extraction.
+++	Examples of bags are stacks, queues, and dequeues.
define BasicBagAggregate(S:BasicType): Category == 
	BasicHomogeneousAggregate S with {
	bag: Generator S -> %;
		++ bag(g) creates a bag by inserting each element in g.
	extract!: % -> S;
		++ extract!(u) destructively removes a (random) item 
		++ from bag u.
	insert!: (S,%) -> %;
		++ insert!(x,u) inserts item x into bag u.
	inspect: % -> S;
		++ inspect(u) returns an (random) element from a bag.
default {

	bag(l:Generator S):% == {
		u:=empty();
		for s in l repeat u:=insert!(s,u);
		u
	}

	inspect(u:%):S == {
		import from Generator S;
		g:=generator u;
		step! g;
		empty? g => error "inspect(u): u is empty";
		value g;
		u pretend S;
		}
	}

}


Foo: BasicHomogeneousAggregate Integer with {
	zzz: Integer -> %;
} == add {
	Rep ==> Integer;
	import from Rep;
	test(x: %): Boolean == rep(x) = 1;
	empty(): % == per 0;
	eq?(a: %, b: %): Boolean == rep(a) = rep(b);
	copy(a: %): % == a;
	generator(x: %): Generator Integer == generate yield rep(x);
	(<<)(t: TextWriter, b:%): TextWriter == t;
	sample: % == per 0;

	zzz(x: Integer): % == per x;
}

t(): () == {
	import from Foo;
	print << (zzz 2 = zzz 3) << newline;
}

t();

