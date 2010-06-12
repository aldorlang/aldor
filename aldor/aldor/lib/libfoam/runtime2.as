#include "foamlib"
#include "runtimelib.as"

InitFn ==> SingleInteger -> Value;
DV ==> DispatchVector;


-- another hack: The recursive call implies that
-- it cannot be inlined from langx 
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

domainMake(df: DomainFun AldorDomainRep): Domain == {
	arep: AldorDomainRep := newAldorDomainRep(df);
	domainMakeDispatch(arep);
}

#if NEVER
domainMakeDispatch(dr: DomainRep): Domain == 
	new dr;

extendMakeDispatch(dr: DomainRep): Domain == 
	newExtend dr;

domainAddExports!(d: DomainRep, names: Array Hash, types: Array Hash,
		  exports: Array Value): () == 
	addExports!(d, names, types, exports);

domainAddDefaults!(d: DomainRep, defaults: CatObj, d2: Domain): () ==
	addDefaults!(d, defaults, d2);

domainAddParents!(d: DomainRep, parents: Array Domain, dAsDom: Domain): () ==
	addParents!(d, parents, dAsDom);

domainAddHash!(d: DomainRep, hash: Hash): () ==
	addHash!(d, hash);

domainAddNameFn!(d: DomainRep, namefn: ()->DomainName): () ==
	addNameFn!(d, namefn);

domainGetExport!(td: Domain, name: Hash, type: Hash): Value == {
	PROFILE(import from StringTable);
	v := getExport!(td, name, type);
	PROFILE(PRINT()<< domainName td << " " << find nameCode
			     <<" "  <<type<<NL());
	v
}

export {
	rtDelayedInit!: (InitFn, SingleInteger) -> (()->Value);
	rtDelayedGetExport!: (Domain, Hash, Hash) -> (()->Value);
} to Foreign(Builtin);

rtDelayedInit!(fn: InitFn, n: SingleInteger)(): Value == 
	fn n;

rtDelayedGetExport!(d: Domain, n1: Hash, n2: Hash)(): Value ==
	domainGetExport!(d, n1, n2);
#endif