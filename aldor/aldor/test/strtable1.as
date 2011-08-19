#include "foamlib"
#include "runtimelib"
#library STBL "stringtable.ao" 

import from STBL;
inline from STBL;

local printDomain(f: (TextWriter, DomainName) -> TextWriter,
      		     out: TextWriter,nm: DomainName): TextWriter == {
	import from List DomainName;
	type nm = ID    => out << name nm;
	type nm = OTHER => out << "??";
	isTuple := type nm = TUPLE;
	lst := args nm;
	if not isTuple then {
		f(out, first lst);
		lst := rest lst;
	}
	out << "(";
	for tail in tails lst repeat {
		arg := first tail;
		f(out, arg);
		not empty? rest tail => out << ", ";
	}
	out << ")";
}

f(x: Int): Int == x;
theStringTable: StringTable := new();

rtAddStrings(a1:Array Hash, a2: Array String): () == { 
	import from StringTable; 
	addNames(theStringTable, a1, a2);
}

failmsg(d: DomainName, name: SingleInteger): () == {
	fn(f: TextWriter, dom: DomainName): TextWriter == printDomain(fn, f, dom);
	printDomain(fn, print, d);
	find(theStringTable, name);
}		
