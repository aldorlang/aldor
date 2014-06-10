#include "aldor"

DomNameType ==>  'ID, APPLY, TUPLE, OTHER';

local ListUtil(T: with): with {
      tails: List T -> List List T;
} == add {
        tails(l: List T): List List T == {
		 if empty? l then empty else cons(l, tails rest l);
	}
}

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

	export from DomNameType;
} == add {
	-- untagged union
	-- !would like Union(a: X, b: X) to work!
	Rep ==> Record(tag: DomNameType, p: Pointer);
	import from Rep, 'l', 's', 't', 'o';
	import from MachineInteger;
	import from List %;
	import from String;
	import from ListUtil %;
	import from List List %;

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
Int		==> MachineInteger;

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

TypeUtils: with {
  typeName: Type -> DomainName;
} == add {

  typeName(T: Type): DomainName == {
	import from Dom;
	getName(T pretend Dom);
  }
}

