#include "runtimelib.as"

import {
	stdoutFile: () -> OutFile;
} from Foreign;

local filePutc(ofile: OutFile)(c: Character):() ==
	write!(ofile, c);

local filePuts(ofile: OutFile)(s: String, start: Int, limit: Int): () ==
	write!(ofile, s, start, limit);

local PRINT(): TextWriter == {
	((): (Character -> (), (String, Int, Int) -> (), ()->Int) +->
	 (filePutc stdoutFile(), filePuts stdoutFile(), ():Int+->0))
	pretend TextWriter;
}

local NL(): Character == {
	import from Machine;
	newline::Character
}

local ERROR(s: String): Exit == {
	fiRaiseException s;	
	never;
}

+++ CatObj is the top-level category representation.
CatObj: Conditional with {
--	new:		CatRep -> %;
--		++ new(cr) creates a new category by wrapping
--		++ a dispatch vector around a CatRep.

	new: (cdv: CatDispatchVector, crep: CatRep) -> %;

	getDefault!:	(%, Domain, Hash, Hash, Box) -> Box;
		++ getDefault!(cat, pcent, name, type, box)
		++ Find a default from the given category,
		++ putting the result in box.  Returns nullBox on failure.

	getParent:	(%, Int) -> %;
		++ getParent(cat, i) finds the i-th parent of cat.
	parentCount: 	% -> Int;
		++ returns the # of parents of the category

	build:		(%, Domain) -> %;

	name:		% -> DomainName;
	hash:		% -> Hash;

--	makeDummy:	() -> %;
	fill!:		(%, %) -> ();
	reFill!:	(%, CatDispatchVector, CatRep) -> ();
}
== add {
	Rep ==> Record (cdv:	CatDispatchVector,
			catRep:	CatRep);

	import from Rep;

	catRep     (cat: %): CatRep		== rep(cat).catRep;
	dispatcher (cat: %): CatDispatchVector	== rep(cat).cdv;

	new (cdv: CatDispatchVector, crep: CatRep) : % == {
	        Nil?(CatDispatchVector) cdv => never;
		per [cdv, crep];
	}

	fill!(cat: %, val: %): () == {
		rep(cat).cdv 	:= dispatcher(val);
		rep(cat).catRep	:= catRep(val);
	}
	
	reFill!(cat: %, dv: CatDispatchVector, r: CatRep): () == {
		rep(cat).cdv 	:= dv;
		rep(cat).catRep	:= r;
	}

#if NOPE		
	getDefault!(cat: %, pcent: Domain, 
		    nm: Hash, type: Hash, box: Box): Box == {
		DEBUG(PRINT() << "(Default lookup: ");
		val := (getter dispatcher cat)(catRep cat, pcent, 
					       nm, type, box);
		val => {
			DEBUG(PRINT() << "Found locally)");
			val
		}
		i := 1;
		l := parentCount(cat);
		DEBUG(PRINT() << "(Iterating " << l << " parents: ");
		while i <= l repeat {
			p := getParent(cat, i);
			if p then {
				val := getDefault!(p, pcent, nm, type, box);
				val => {
					DEBUG(PRINT() << " Found))");
					return val;
				}
				DEBUG(PRINT() << " Failed))");
			}
			i := i + 1;
		}
		DEBUG(PRINT() << " Failed))");
		nullBox();
	}

#endif	
	getDefault!(cat: %, pcent: Domain, 
		    nm: Hash, type: Hash, box: Box): Box == {
		stack: List % := nil();
		stack := cons(cat, stack);
		DEBUG(PRINT() << "(Default lookup: ");

		while (not empty? stack) repeat {
			cat := first(stack);
			stack := rest(stack);		      
			val := (getter dispatcher cat)(catRep cat, pcent, 
		      	  			       nm, type, box);
                        val => {
			  DEBUG(PRINT() << "Found locally)");
			  return val
		        }
	
		      	l := parentCount(cat);
			DEBUG(PRINT() << "(Iterating " << l << " parents: ");
			while l > 0 repeat {
			      p := getParent(cat, l);
			      if p then {
			          stack := cons(p, stack);
			      }
			      l := l - 1;
			}
			DEBUG(PRINT() << " Added))");
		}
		DEBUG(PRINT() << "Not found)");
		nullBox();
	}


	parentCount(cat: %): Int == (parentCounter dispatcher cat)(catRep cat);

	getParent(cat: %, i: Int): % ==
		(parentGetter dispatcher cat)(catRep cat, i) pretend %;

	build(cat: %, dom: Domain): % == 
		(builder dispatcher cat)(catRep cat, dom) pretend %;

	name(cat: %): DomainName ==
		(namer dispatcher cat)(catRep cat);

	hash(cat: %): Hash ==
		(hasher dispatcher cat)(catRep cat);

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}
