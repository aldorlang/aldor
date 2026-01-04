#include "foamlib"
#include "runtimelib"

-- This program fails to compile (with optimisatio on).  Reason is
-- that the inliner keeps inlining printDomain into failmsg - it looks
-- like the test for a recursive call isn't strong enough, or there
-- should be something that inlines functions together properly (hmm,
-- not impossible, as they must form a clique).

local printDomain(out: TextWriter,nm: DomainName): TextWriter == {
	import from List DomainName;
	type nm = ID    => out << name nm;
	type nm = OTHER => out << "??";
	isTuple := type nm = TUPLE;
	lst := args nm;
	if not isTuple then {
		printDomain(out, first lst);
		lst := rest lst;
	}
	out << "(";
	for tail in tails lst repeat {
		arg := first tail;
		printDomain(out, arg);
		not empty? rest tail => out << ", ";
	}
	out << ")";
}

theStringTable: StringTable := new();

rtAddStrings(a1:Array Hash, a2: Array String): () == { 
	import from StringTable; 
	addNames(theStringTable, a1, a2);
}

failmsg(d: DomainName, name: SingleInteger): () == {
	printDomain(print, d);
	find(theStringTable, name);
}		
