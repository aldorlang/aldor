-----------------------------------------------------------------------------
----
---- langx.as:  Extensions to AXIOM-XL language defined types.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

--!! These defintions are wrong, but allow List Record etc to be instantiated.

import { formatSInt: BSInt -> String } from Foreign;

import from Machine;

extend Record(T: Tuple Type): BasicType with
== add {
	Rep ==> BPtr;
	import from String;
	default a, b: %;

	a = b: Boolean == (rep a = rep b)::Boolean;
	sample: %      == per nil;
	(outp: TextWriter) << (a: %): TextWriter == outp << "Record()";
}

extend Union(T: Tuple Type): BasicType with
 == add {
	Rep ==> Record(n: BSInt, o: Pointer);
	import from Tuple Type;
	import from Rep;
	import from SingleInteger;
	import from String;
	default a, b: %;

	a = b: Boolean == (a pretend BPtr = b pretend BPtr)::Boolean;

	-- This won't work (bug methinks): sample: % == per sample
	sample: % == nil@BPtr pretend %;


	(outp: TextWriter) << a: TextWriter == {
		import from DomainName;
		-- unions are zero based, tuples aren't
		ThisType ==> element(T, coerce idx);
		idx: BSInt == rep(a).n + 1;
		outp << "U[";
		write!(outp, formatSInt  idx);
		outp <<", ";
		if ThisType has BasicType then {
			import from ThisType;
			outp << (rep(a).o) pretend ThisType;
		}
		else 
			outp << "??";
		outp << "@" << typeName(ThisType) << "]";
	}
}

extend Enumeration(T: Tuple Type): BasicType  with
 == add {
	Rep ==> BSInt;
	import from SingleInteger, Boolean;
	-- this is dealt with internally
	local TT(): Tuple String == T pretend Tuple String;
	
	(a: %) = (b: %): Boolean == (rep(a) = rep(b))::Boolean;
	sample: % == per 0;
	(outp: TextWriter) << (a: %): TextWriter == {
		import from Tuple String;
		outp << element(TT(), rep(a)::SingleInteger + 1);
	}
}

DomNameType ==>  'ID, APPLY, TUPLE, OTHER';

DomainName: with {
	type:  % -> DomNameType;
	name:  % -> String;
	args:  % -> List %;
	tuple: % -> List %;
	<<: (TextWriter, %) -> TextWriter;
	noName:  ()	 -> %;
	new:     String  -> %;
	other:	 Pointer -> %;

	combine: (Boolean, List  %) -> %;
	combine: (Boolean, Tuple %) -> %;

	fullName: % -> List String;

	export from DomNameType;
} == add {
	-- untagged union 
	-- !would like Union(a: X, b: X) to work!
	Rep ==> Record(tag: DomNameType, p: Pointer);
	import from Rep, 'l', 's', 't', 'o';
	import from SingleInteger;
	import from List %;

	default nm: %;

	local apply(x: Rep, l: 's'):  String == x.p pretend String;
	local apply(x: Rep, l: 'l'):  List % == x.p pretend List %;
	local apply(x: Rep, l: 't'):  List % == x.p pretend List %;
	local apply(x: Rep, l: 'o'): Pointer == x.p;

	local (x: Rep) case (l: 's'): Boolean == x.tag = ID;
	local (x: Rep) case (l: 'l'): Boolean == x.tag = APPLY;
	local (x: Rep) case (l: 't'): Boolean == x.tag = TUPLE;
	local (x: Rep) case (l: 'o'): Boolean == x.tag = OTHER;
	
	local [s: String]:  	   Rep == [ID, s pretend Pointer];
	local [v:'l', x: List %]:  Rep == [APPLY, x pretend Pointer];
	local [v:'t', l: List %]:  Rep == [TUPLE, l pretend Pointer];
	local [o: Pointer]: 	   Rep == [OTHER, o];

	type nm: DomNameType == rep(nm).tag;

	name nm:  String == rep(nm).s;
	args nm:  List % == rep(nm).l;
	tuple nm: List % == rep(nm).t;

	(out: TextWriter) << nm: TextWriter == {
		type nm = ID    => out << name nm;
		type nm = OTHER => out << "??";
		isTuple := type nm = TUPLE;
		lst := if isTuple then tuple nm else args nm;
		if not isTuple then {
			out << first lst;
			lst := rest lst;
		}
		out << "(";
		for tail in tails lst repeat {
			arg := first tail;
			out << arg;
			not empty? rest tail => out << ", ";
		}
		out << ")";
	}

	fullName(nm):List String == {
		type nm = ID    => [name nm];
		type nm = OTHER => ["??"];
		isTuple := type nm = TUPLE;
		lst := if isTuple then tuple nm else args nm;
		res := empty()$List(String);
		if not isTuple then {
			res := fullName(first lst);
			lst := rest lst;
		}
		res := cons("(", res);
		for tail in tails lst repeat {
			arg := reverse fullName(first tail);
			for part in arg repeat
				res := cons(part, res);
			not empty? rest tail =>
				{ res := cons(", ", res); }
		}
		cons(")", res);
	}

	new(s: String):      % == per [s];
	other(p: Pointer):   % == per [p];
	combine(isTuple: Boolean, tup: Tuple %): % == {
		if isTuple then per [t, [tup]]
		else per [l, [tup]];
	}
	combine(isTuple: Boolean, lst: List %):  % == {
		if isTuple then per [t, lst]
		else per [l, lst];
	}
	
	noName(): % == new "Dunno";
}	

-- Nasty implementation details...

DomNamer	==> DomainRep -> DomainName;
DomainRep	==> Pointer;
Reserved	==> Pointer;
Int		==> SingleInteger;

local Dom, DispatchVector;

Dom: with {
	getName:		%->DomainName;
		++ getName(dom) returns the name of a domain
}
== add {
	Rep ==> Record (dispatcher:	DispatchVector, 
			domainRep:	DomainRep);
	import from Rep;
	domainRep  (td: %): DomainRep		== rep(td).domainRep;
	dispatcher (td: %): DispatchVector	== rep(td).dispatcher;

	getName(td: %): DomainName ==
		(namer dispatcher td)(domainRep td);
}

DispatchVector: with {
	namer:		%->  DomNamer;
}
== add {
	Rep ==> Record(tag: Int,
		       namer:  DomNamer,
		       noname:  Reserved,
                       getter: Reserved,
		       hasher: Reserved,
		       inheriter: Reserved);

	import from Rep;

	namer(dv: %) : DomNamer  == rep(dv).namer;
}

typeName(T: Type): DomainName == {
	import from Dom;
	getName(T pretend Dom);
}


IsPackage: with {
	is: (with, with) -> Boolean;
} == add {
	-- further obfuscation welcome.
	You(X: with): Category == with;
	Me(OW: with): You(OW) with == add;

	is(Tarzan: with, Jayne: with): Boolean == Me(Tarzan) has You(Jayne);
}


#if 0
-- Unused, as we can't yet say much about the internals of a record.

extend Record(T: Tuple BasicType): BasicType == add {
	Rep ==> PrimitiveArray(length T);
	
	nth(x: %, n: SingleInteger) == per(x).n;
	(a: %) = (b: %): Boolean ==
		for i in 1..length T repeat {
			import from elt(T,i);
			nth(a, i) pretend elt(T,i) = nth(b, i) pretend elt(T,i);
		}
}

#endif
