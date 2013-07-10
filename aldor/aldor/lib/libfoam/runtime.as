-- Runtime support library for Axiomxl.

-- Contains the runtime implementation of domains, and other callbacks
-- made by the code generator.

-- Do not change the interface to these functions without also changing
-- the calls made to them in genfoam.c.

-- This file must be compiled with "-O -Q inline-all -W runtime" when
-- making the axiomxl runtime library.

-- Because this file implements the code for getting exports from domains,
-- it needs to generate FOAM code that has no "gets" in it, otherwise
-- it would loop infinitely when instantiating itself.  Because of this,
-- the "-W runtime" flag causes it to compile with a flat environment
-- for its types, so we have to disallow parameterized types here.  We
-- also need to avoid direct recursion, but indirect-recursion, through
-- function pointers is used for getting exports from a parent in
-- an "add" chain. 

#include "foamlib"

--#assert ExtendReplace
-- #assert DebugGets
-- #assert ProfGets

#assert DynamicCache

#if DebugGets
DEBUG(x) ==> x;
#else
DEBUG(x) ==> ();
#endif

#if ProfGets
PROFILE(x) ==> x;
#else
PROFILE(x) ==> ();
#endif

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil @ Ptr pretend S;
	Nil?(S)(x)	== x @ S pretend Ptr = nil;
	Reserved	== Pointer;
}

import from Ptr, Int;

-- Hash code type.
macro {
	Hash		== Int;
	wildcard	== -1;
}


-- declare all domains as local
local Domain, DomainRep, PointerDomain, DispatchVector;
local CatObj, CatRep, PointerCatObj, CatDispatchVector;
local Box, LazyImport, StringTable, BasicTuple, PtrCache;
#if DynamicCache
local VariablePtrCache, CacheTable;
#endif

-- Value is used for types stored in a single word.  All domain exports
-- must be storable in this type.
local Value: BasicType == Ptr;

macro {
	ObjectTyper	== () -> Int;
	ObjDomain	== 0;
	ObjCategory	== 10;
}

macro {
	DomainFun S	== S -> (S, Hash) -> ();
	DomGetter 	== (DomainRep, Domain, Hash, Hash, Box, Bit) -> Box;
	DomHasher	== DomainRep -> Hash;
	DomNamer	== DomainRep -> DomainName;
	DomInheritTo    == (DomainRep, Domain) -> Domain;
}

macro {
	CatRepInit S		== (S, Domain) -> ();
	CatBuilder		== (CatRep, Domain) -> CatObj;
	CatGetter		== (CatRep, Domain, Hash, Hash, Box) -> Box;
	CatHasher		== CatRep -> Hash;
	CatParentCounter	== CatRep -> Int;
	CatParentGetter		== (CatRep, Int) -> CatObj;
	CatNamer		== (CatRep) -> DomainName;
}

-- hashCode of "%%"
pcentPcentHash ==> 51482908;

-- some local functions
local checkGetCache, addGetCache;

--!! This unmitigated hack is to get around inliner limitation to functions.
--!!\begin{hack}
import {
	stdoutFile: () -> OutFile;
} from Foreign;

local filePutc(ofile: OutFile)(c: Character):() ==
	write!(ofile, c);

local filePuts(ofile: OutFile)(s: String, start: Int, limit: Int): () ==
	write!(ofile, s, start, limit);

local ERROR(s: String): Exit == {
	fiRaiseException s;	
	never;
}

local PRINT(): TextWriter == {
	((): (Character -> (), (String, Int, Int) -> (), ()->Int) +->
	 (filePutc stdoutFile(), filePuts stdoutFile(), ():Int+->0))
	pretend TextWriter;
}

local NL(): Character == {
	import from Machine;
	newline::Character
}

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

local (out: TextWriter) << (nm: DomainName): TextWriter == printDomain(out, nm);
--!!\end{hack}
	
export {
	-- Functions for creating and enriching axiomxl domains.

	domainMake:		DomainFun(DomainRep) -> Domain;
		++ domainMake(fun) creates a new lazy domain object.

	domainMakeDispatch:	DomainRep -> Domain;
		++ domainMakeDispatch(dr) wraps a dispatch vector
		++ around a DomainRep.

	domainAddExports!:	(DomainRep,
				 Array Hash, Array Hash, Array Value) -> ();
		++ domainAddExports!(dom, names, types, exports)
		++ Set the exports of a domain.

	domainAddDefaults!:	(DomainRep, CatObj, Domain) -> ();
		++ domainAddDefaults!(dom, defaults, dom)
		++ Sets the default package for a domain.

	domainAddParents!:	(DomainRep, Array Domain, Domain) -> ();
		++ defaultsAddExports!(dom, parents)
		++ Set the parents of a default package.

	domainAddHash!:		(DomainRep, Hash) -> ();
		++ domainAddHash!(dom, hash) sets the hash code of a domain.

	domainAddNameFn!: 	(DomainRep, ()->DomainName)->();
		++ sets the domains naming function

	domainGetExport!:	(Domain, Hash, Hash) -> Value;
		++ domainGetExport!(dom, name, type)
		++ Gets an export from a domain, given the hash codes for
		++ its name and type.  Takes a hard error on failure.

	domainTestExport!:	(Domain, Hash, Hash) -> Bit;
		++ domainTestExport!(dom, name, type)
		++ returns true if the given export exists in dom

	domainHash!:		Domain -> Hash;
		++ domainHash!(dom) returns the hash code from a domain.

	domainName: 		Domain -> DomainName;
		++ domainName returns the name of a domain
	domainMakeDummy:		() -> Domain;
	domainFill!:		(Domain, Domain) -> ();

	-- Functions for creating and enriching axiomxl categories.

	categoryAddParents!:	(CatRep, Array CatObj, CatObj) -> ();
		++ categoryAddExports!(dom, parents, self)
		++ Set the parents of a default package.
		++ additional arg is for uniformity
	categoryAddNameFn!: (CatRep, ()->DomainName) -> ();
		++ Sets the name of a category.

	categoryAddExports!:	(CatRep,
				 Array Hash, Array Hash, Array Value) -> ();
		++ categoryAddExports!(dom, names, types, exports)
		++ Set the exports of a category.

	categoryMake:		(CatRepInit(CatRep), ()->Hash, ()->DomainName) -> CatObj;
		++ Constructing new cats

	categoryBuild:		(CatObj, Domain) -> CatObj;

	categoryName:		CatObj -> DomainName;
		++ Returns the name of a category
	categoryMakeDummy:		() -> CatObj;
	categoryFill!:		(CatObj, CatObj) -> ();

	-- Utility functions called from code generation.

	noOperation:		() -> ();
		++ Do nothing --- used to clobber initialisation fns.

	extendMake:		DomainFun(DomainRep) -> Domain;
		++ extendMake(fun) creates a new lazy extend domain object;
	extendFill!:	(DomainRep, Array Domain) -> ();
		++ adds the extendee, extender pair to an extension domain
	lazyGetExport!:		(Domain, Hash, Hash) -> LazyImport;
		++ creates a lazy function to retrieve the export
	lazyForceImport: 	LazyImport->Value;
		++ forces a get on the lazy value
	rtConstSIntFn: SingleInteger->(()->SingleInteger);
		++ Save on creating functions.
	rtAddStrings: (Array Hash, Array String) -> ();
		++ Adds more strings to the list of known exports
	domainPrepare!:		Domain -> ();
		++ initializes a domain.
} to Foreign(Builtin);

+++ Domain is the top-level domain representaion, designed to operate in
+++ an environment of mixed runtime systems.  The domain consists of
+++ a pointer to the domain's native representation, and a vector of
+++ functions for accessing it.  Currently only "get" and "hash" functions
+++ are required.
Domain: Conditional with {
	new:			DomainRep -> %;
		++ new(dr) creates a new domain by wrapping
		++ a dispatch vector around a DomainRep.

#if ExtendReplace
	copy:			% -> %;
		++ copy(dom) creates a new domain object using
		++ the dispatch vector and DomainRep from dom.

	ncopy:			(%, %) -> ();
		++ ncopy(dst, src) destructively copies
		++ the dispatch vector and DomainRep from src to dst.
#endif

	newExtend:		DomainRep -> %;
		++ extend(dr) creates a new domain by wrapping
		++ the dispatch vector for extensions around a DomainRep.

--	prepare!:		% -> ();
--		++ prepare(dom) forces a domain to fully instantiate.

	getExport!:		(%, Hash, Hash) -> Value;
		++ getExport!(dom, name, type) gets an export from a domain, 
		++ given the hash codes for its name and type.  Takes a hard 
		++ error on failure.

	getExportInner!:	(%, %, Hash, Hash, Box, Bit) -> Box;
		++ getExportInner!(dom, name, type, box, skipDefaults)
		++ Fetch an export from the given domain, putting the result
		++ in the box.  It won't look in category default packages if
		++ if skipDefaults is true.  Returns nullBox on failure.

	getHash!:		% -> Hash;
		++ getHash!(dom) returns the hash code for a domain.

	testExport!:		(%, Hash, Hash) -> Bit;
		++ testExport!(dom, name, type) tests for an 
		++ export with the given name and type in the domain
	getName:		%->DomainName;
		++ getName(dom) returns the name of a domain
	inheritTo: 		(%, Domain)->Domain;
		++ returns an object suitable for being a parent of dom2
		++ This function is so that A# can have a single function
		++ for both computing the hashcodes of types and initialising 
		++ a domain.  Really ought to be expunged.
	makeDummy: 	() -> %;
		++ specialized domain creators

	fill!:		(%, %) -> ();
	reFill!:	(%, DispatchVector, DomainRep) -> ();
	domainRep:	% -> DomainRep;
}
== add {
	Rep ==> Record (dispatcher:	DispatchVector, 
			domainRep:	DomainRep);

	import from Rep;
	import from PointerDomain;

	domainRep  (td: %): DomainRep		== rep(td).domainRep;
	dispatcher (td: %): DispatchVector	== rep(td).dispatcher;

	new (d: DomainRep) : % ==
		per [axiomxlDispatchVector(), d];

	new (dv: DispatchVector, d: DomainRep) : % ==
		per [dv, d];

	newExtend (d: DomainRep) : % ==
		per [extendDispatchVector(), d];

	makeDummy(): % == per [dummyDispatchVector(), Nil DomainRep];

	fill!(dom: %, val: %): () == {
		rep(dom).dispatcher := dispatcher(val);
		rep(dom).domainRep  := domainRep(val);
	}

	reFill!(dom: %, dv: DispatchVector, rp: DomainRep): () == {
		rep(dom).dispatcher := dv;
		rep(dom).domainRep  := rp;
	}

#if ExtendReplace
	copy (td: %) : % ==
		per [dispatcher td, domainRep td];

	ncopy (dst: %, src: %) : () == {
		rep(dst).dispatcher := rep(src).dispatcher;
		rep(dst).domainRep  := rep(src).domainRep;
	}
#endif

--	prepare! (td: %) : () ==
--		prepare! domainRep td;

	-- Create a box to use for all calls to getExport.
	local box: Box := new Nil Value;

	getExport0! (td: %, name: Hash, type: Hash) : Box == {
		-- check to make sure that we haven't got a category..
		-- we need a more generic technique than this.
		--tag dispatcher td = ObjCategory => nullBox();
		get := getter dispatcher td;
		val := get(domainRep td, td pretend Domain, 
			   name, type, box, false);
		val
	}

	getExport! (td: %, name: Hash, type: Hash) : Value == {
		import from StringTable;
		val := getExport0!(td, name, type);
		val => value val;
		failmsg(td, name, type);
		ERROR "Export not found";
	}
	failmsg(td: %, name: Hash, type: Hash): () == {
		import from StringTable;
		PRINT() << "Looking in ";
		printDomain(PRINT(), getName td)
                 	<< " for "       << find name
                        << " with code " << type << NL();
	}		

	getExportInner! (td: %, pcent: %, name: Hash, type: Hash, box: Box, skip: Bit)
			: Box ==
		(getter dispatcher td)(domainRep td, pcent pretend Domain, name, type, box, skip);

	getHash! (td: %) : Hash ==
		(hasher dispatcher td)(domainRep td);

	testExport! (td: %, name: Hash, type: Hash) : Bit ==
		test getExport0!(td, name, type);

	getName(td: %): DomainName ==
		(namer dispatcher td)(domainRep td);

	inheritTo(td: %, dom2: Domain): Domain == 
		(inheriter dispatcher td)(domainRep td, dom2);

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

+++ DomainRep defines the run-time representation of axiomxl domains.  Domains
+++ are lazy.  Initially domains only hold a function which, when called,
+++ fills in the hash code for the domain, and sets another function.
+++ When this funcation is called, the parent and export fields are set, and
+++ any code from the "add" body is run.
+++ Domains cache the last few lookups (this gives up to 10% speedup, 
+++ depending on the example.
DomainRep: Conditional with {
	new:		DomainFun % -> %;
		++ new(fun) creates a new domain.

	prepare!:	% -> ();
		++ prepare!(dom) forces a domain to fully instantiate.

	addExports!:	(%, Array Hash, Array Hash, Array Value) -> ();
		++ addExports!(dom, names, types, exports)
		++ sets the exports fields of a domain.

	addDefaults!:	(%, CatObj, Domain) -> ();
		++ addDefaults!(dom, defaults, domain)
		++ sets the default package for a domain.
		++ Additional arg is the wrapped domain.

	addParents!:	(%, Array Domain, Domain) -> ();
		++ addParents!(dom, parents) sets the parent field of a domain.

	addHash!:	(%, Hash) -> ();
		++ addHash!(dom, hash) sets the hash field of a domain.

	addNameFn!:	(%, ()->DomainName) -> ();
		++ addName!(dom, name) sets the naming fn  of a domain.

	get:		(%, Domain, Hash, Hash, Box, Bit) -> Box;
		++ get(dom, name, type, box, skipDefaults)
		++ Fetch an export from the given domain, putting the result
		++ in the box.  It won't look in category default packages if
		++ if skipDefaults is true.  Returns nullBox on failure.
		++ If the type hash code is wildcard, then any type is OK.

	hash:		% -> Hash;
		++ hash(dom) returns the hash code for a domain.

	getExtend:	(%, Domain, Hash, Hash, Box, Bit) -> Box;
		++ get(dom, name, type, box, skipDefaults)
		++ Fetch an export from an extended domain.

	hashExtend:	% -> Hash;
		++ hashExtend(dom) returns the hash code for extended domains.
	extendFillObj!: (%, Array Domain) -> ();
		++ fills extend info
	axiomxlDispatchVector: () -> DispatchVector;
		++ axiomxlDispatchVector() creates the dispatch vector for 
		++ axiomxl domains.

	extendDispatchVector: () -> DispatchVector;
		++ extendDispatchVector() creates the dispatch vector for
		++ extended domains.
	dummyDispatchVector: () -> DispatchVector;
}
== add {
	Rep1 ==> Ptr;
	Per1 ==> DomainFun %;
	rep1 x ==> x @ Per1 pretend Rep1;
	per1 x ==> x @ Rep1 pretend Per1;

	Rep2 ==> Ptr;
	Per2 ==> (%, Hash) -> ();
	rep2 x ==> x @ Per2 pretend Rep2;
	per2 x ==> x @ Rep2 pretend Per2;

	Rep ==> Record (f1:		Rep1,
			f2:		Rep2,
			hashcode:	Hash, 
			parents:	Array Domain,
			extendees:	Array Domain,
			defaults:	CatObj,
			names:		Array Hash,
			types:		Array Hash, 
			exports:	Array Value,
			ngets:		Int,
			serial: 	SingleInteger,
			cache:		PtrCache,
			nameFn: ()->DomainName);

	import from Rep;

	local serialThis: SingleInteger := 0;
	domainStdName(): DomainName == noName();

	new(fn1: DomainFun %): % == {
		free serialThis;
		serialThis := serialThis + 1;
		per [rep1 fn1, rep2 Nil Per2, 0, 
		     Nil Array Domain, Nil Array Domain, Nil CatObj,
		     Nil Array Hash, 
		     Nil Array Hash, Nil Array Value, 0,
		     serialThis, newCache(), domainStdName ]
	}
	prepare!(dom: %): () == {
#if ExtendReplace
		prepareHash!(dom);
#endif
		prepareGetter!(dom);
	}

	addExports!(dom: %, nams: Array Hash, typs: Array Hash,
		    expts: Array Value): () == {
		rep(dom).names    := nams;
		rep(dom).types    := typs;
		rep(dom).exports  := expts;
	}


	addDefaults!(dom: %, defs: CatObj, domAsDomain: Domain): () ==
		rep(dom).defaults := categoryBuild(defs, domAsDomain);

	addParents!(dom: %, parnts: Array Domain, child: Domain): () == {
		for i in 1..#parnts repeat
			parnts.i := inheritTo(parnts.i, child);
		rep(dom).parents  := parnts;
	}

	addHash!(dom: %, code: Hash): () ==
		rep(dom).hashcode := code;

	addNameFn!(dom: %, namefn: ()->DomainName): () == {
		rep(dom).nameFn := namefn;
	}

	local prepareGetter!(dom: %): () == {
		h := hash(dom);
		if Nil?(Array Hash)(rep(dom).names) then
			per2(rep(dom).f2)(dom, h);
	}

	prepareHash!(dom: %): () == {
		if Nil?(Rep2)(rep(dom).f2) then {
			rep(dom).f2 := rep2(per1(rep(dom).f1)(dom));
			DEBUG(PRINT()<<"Initialised: "  <<name dom
			             <<" with hashcode "<<rep(dom).hashcode
				     <<NL());
		}
	}

	get(dom: %, pcent: Domain, nameCode: Hash, type: Hash, 
	    box: Box, skip: Bit): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		(newBox, flag, key) := checkGetCache(rep(dom).cache, 
						    pcent, nameCode, type, skip);
		flag => {
			not newBox => newBox;
			setVal!(box, value newBox); 
			box;
		}
		DEBUG(PRINT()<<"(Looking for "<<find nameCode
			     <<" with code "  <<type<<NL());
		rep(dom).ngets := rep(dom).ngets + 1;
		if rep(dom).ngets > 10 then {
			printDomain(PRINT(), name dom) << NL();
			ERROR "Circular get broken";
		}
		if (nameCode = pcentPcentHash) then 
			newBox := getauxPcentPcent(dom, pcent, type, box, skip);
		else 
			newBox := getaux(dom, pcent, nameCode, type, box, skip);
		rep(dom).ngets := rep(dom).ngets - 1;
		DEBUG(PRINT()<<(if newBox then "OK " else "Fail ")
		             <<"from "<<name dom<<")");
		addGetCache(rep(dom).cache, key, newBox);
		newBox;
	}

	getaux(dom: %, pcent: Domain, nameCode: Hash, type: Hash, 
	       box: Box, skip: Bit): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		local newBox: Box;
		DEBUG(PRINT() << "(Preparing " << name dom << ")" << NL());
		prepareGetter! dom;
		DEBUG(PRINT() << "(" << name dom << " is prepared)" << NL());
		for expName  in rep(dom).names 
		for expType  in rep(dom).types
		for expValue in rep(dom).exports
	        repeat {
			Nil?(Hash)expName => iterate;

			DEBUG(PRINT() << "(Export: " << find expName
				      << " with type " << expType << NL());

			nameCode=expName and (type=expType or type=wildcard) => {
				setVal!(box, expValue);
				return box;
			}
		}

		DEBUG(PRINT()<<"(Not found "<<find nameCode
			     <<" with type "  <<type
			     <<" in " << name dom << ")" <<NL());

		-- call in order:
		-- parents (skipping defaults)
		-- extends (skipping defaults)
		-- defaults
		-- parents (defaults)
		-- extents (defaults)
		count := if skip then 1 else 2;
		skip  := true;
		for i in 1..count repeat {
			def := rep(dom).defaults;
			if not skip and def then {
				newBox := getDefault!(def, pcent, nameCode, type, box);
				newBox => return newBox;
			}
			pars := rep(dom).parents;
			newBox := getauxArray(pars, pcent, nameCode, type, box, skip);
			newBox => return newBox;
	
			exts := rep(dom).extendees;
			newBox := getauxArray(exts, pcent, nameCode, type, box, skip);
			newBox => return newBox;

			skip := not skip;
		}

		nullBox();
	}

	getauxArray(doms: Array Domain, pcent: Domain, name: Hash, 
		    type: Hash, box: Box, skip: Bit): Box == {
		Nil?(Array Domain)(doms) => nullBox();
		for dom in doms repeat {
			if dom then {
				newBox := getExportInner!(dom, pcent, name,
							 type, box, skip);
				newBox => return newBox;
			}
		}
		return nullBox();
	}

	getauxPcentPcent(dom: %, pcent: Domain, type: Hash, 
			 box: Box, skip: Bit): Box == {
		prepareGetter! dom;
		def := rep(dom).defaults;
		not def => return nullBox();
		newBox := getDefault!(def, pcent, pcentPcentHash, type, box);
		newBox => newBox;
		exts := rep(dom).extendees;
		Nil?(Array Domain)(exts) => nullBox();
		for ext in exts repeat {
			if ext then {
				newBox := getExportInner!(
						ext, pcent, 
						pcentPcentHash,
						type, box, skip);
				newBox => return newBox;
			}
		}
		nullBox();
	}

	hash(dom: %): Hash == {
		prepareHash! dom;
		rep(dom).hashcode;
	}

	name(dom: %): DomainName == {
		prepareHash! dom;
		(rep(dom).nameFn)();
	}

	inheritTo(dom: %, child: Domain): Domain == {
		fn1(self: %): ((%, Hash) -> ()) == {
			prepareHash!(dom);
			addHash!(self, domainHash!(child));
			addNameFn!(self, rep(dom).nameFn);
			if not Nil?(Array Domain)(rep(dom).extendees) then {
				arr := new(#(rep(dom).extendees), Nil(Domain));
				for i in 1..#arr repeat
				       arr.i := inheritTo(rep(dom).extendees.i,
				 			  child);
				rep(self).extendees    := arr;
			}
			rep(self).cache        := newCache();
			per2(rep(dom).f2);
		}
		dr := new(fn1);
		domainMakeDispatch(dr pretend DomainRep)
	}

	-- Operations for extension domains.

	getExtend (dom: %, pcent: Domain, name: Hash, type: Hash, 
		   box: Box, skip: Bit): Box
	== {
		local newBox: Box;
		prepareHash! dom;
		pars  := rep(dom).parents;
		newBox := getauxArray(pars, pcent, name, type, box, skip);
		newBox => return newBox;
		nullBox();
	}

	hashExtend (dom: %): Hash == {
		prepareHash! dom;
		pars := rep(dom).parents;
		if not Nil?(Array Domain)(pars) then
			getHash! pars.1;
		else
			0@Hash;
	}

	nameExtend(dom: %): DomainName == {
		prepareHash! dom;
		pars := rep(dom).parents;
		if not Nil?(Array Domain)(pars) then
			domainName pars.1;
		else
			noName();
	}
	inheritToExtend(dom: %, child: Domain): Domain == {
		fn1(self: %): ((%, Hash) -> ()) == {
			local arr: Array Domain;
			prepareHash!(dom);
			arr := new(#(rep(dom).parents), Nil(Domain));
			for i in 1..#arr repeat arr.i := rep(dom).parents.i;
			addParents!(self, arr, child);
			if not Nil?(Array Domain)(rep(dom).extendees) then {
				arr := new(#(rep(dom).extendees), Nil(Domain));
				for i in 1..#arr repeat
					arr.i := inheritTo(rep(dom).extendees.i, 
						           child);
				rep(self).extendees := arr;
			}
			rep(self).cache     := newCache();
			(x: %, h: Hash): () +-> {} };
		dr:=new(fn1);
		extendMakeDispatch(dr pretend DomainRep);
	}
	extendFillObj!(dom: %, pars: Array Domain): () == {
		rep(dom).parents := pars;
#if ExtendReplace
		ndom := extendMakeDispatch(dom pretend DomainRep);
		for i in 1..#pars repeat {
			par := pars.i;
			prepare!(par);
			npar := copy par;
			ncopy(par, ndom);
			pars.i := npar;
		}
#endif
		--!! The first domain must be the thing being extended
		-- this forces all extends to have adds on the RHS
		last := pars.1;
		extendFillExtender!((domainRep last) pretend %, pars, #pars);
	}

	extendFillExtender!(dom: %, pars: Array Domain, n: Int): () == {
		exts : Array Domain := new(n-1, Nil Domain);
		for i in 2..n repeat
			exts.(i-1) := pars.i;
		rep(dom).extendees := exts;
	}

	-- dummied operations

	nameDummy(dom: %): DomainName == noName();

	getDummy(dom: %, pcent: Domain, name: Hash, 
		 type: Hash, box: Box, skip: Bit): Box == 
		ERROR 	"get on incomplete domain";

	hashDummy(dom: %): Hash == 
		ERROR 	"hash value read on incomplete domain";

	inheritToDummy(dom: %, child: Domain): Domain == 
		ERROR "inherit to incomplete domain";

	-- Dispatch vector creation operations.

	--!! These functions are here rather than in DispatchVector so
	--!! we can initialize the functions used to construct the vector
	--!! using global constants instead of using calls to domainGetExport!.

	DV ==> DispatchVector;

	local adv: DV := Nil DV;
	local edv: DV := Nil DV;
	local ddv: DV := Nil DV;

	axiomxlDispatchVector () : DV == {
		free adv: DV;
		not adv => adv := new(name@(%->DomainName) pretend DomNamer,
				      get pretend DomGetter,
				      hash@(%->Hash) pretend DomHasher,
				      inheritTo@((%, Domain)->Domain)
						  pretend DomInheritTo
					);
				     		adv;
	}

	extendDispatchVector () : DV == {
		free edv: DV;
		not edv => edv := new(nameExtend pretend DomNamer,
			 	      getExtend pretend DomGetter,
				      hashExtend pretend DomHasher,
				      inheritToExtend pretend DomInheritTo
				 );
				      		edv;
	}

	dummyDispatchVector () : DV == {
		free ddv: DV;
		not ddv => ddv := new(nameDummy pretend DomNamer,
			 	      getDummy pretend DomGetter,
				      hashDummy pretend DomHasher,
				      inheritToDummy pretend DomInheritTo
				 );
				ddv;
	}

	
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

-- hack 'cops I cannae put it inside...

PointerDomain: with {
	pointerDV: () -> DispatchVector;
	new:	   (() -> Domain) -> DomainRep;
} == add {
	Rep ==> Union(init: ()->Domain, dom: Domain);
	DV  ==> DispatchVector;
	import from Rep;
	pdv: DispatchVector := Nil DispatchVector;
	new(fn: () -> Domain): DomainRep == [fn] pretend DomainRep;

	deref(d: %): Domain == {
		if rep(d) case init then
			rep(d).dom := rep(d).init();
		rep(d).dom;
	}

	name(d: %): DomainName == 
		domainName(deref d);
	
	domHash(d: %): Hash == domainHash!(deref d);

	get(d: %, pc: Domain, n1: Hash, n2: Hash, box: Box, skip: Bit): Box == 
		getExportInner!(deref d, pc, n1, n2, box, skip);

	inheritTo(d: %, d2: Domain): Domain == {
		import from Domain;
		if rep(d) case dom then
			inheritTo(rep(d).dom, d2)
		else {
			fn(): Domain == inheritTo(deref d, d2);
			rtLazyDomFrFn(fn);
		}
	}

	pointerDV() : DV == {
		free pdv: DV;
		not pdv => pdv := new(name@(%->DomainName) pretend DomNamer,
				      get pretend DomGetter,
				      domHash@(%->Hash) pretend DomHasher,
				      inheritTo@((%, Domain)->Domain)
						  pretend DomInheritTo
					);
		pdv;
	}

}



+++ Structure containing a domain's protocol for getting exports and
+++ producing hash codes.  This is in a separate structure to accomodate
+++ mixed runtime environments.
DispatchVector: Conditional with {
	new:		(DomNamer, DomGetter, DomHasher, DomInheritTo) -> %;
		++ new(get, hash) constructs a dispatch vector.

	getter:		% -> DomGetter;
		++ getter(dv) returns the getter function.

	hasher:		% -> DomHasher;
		++ hasher(dv) returns the hash code function.

	namer:		%->  DomNamer;
		++ namer(dv) returns the function giving the name of a domain
	tag: 		% -> Int;
	reserved:	% -> Reserved;
	inheriter:	% -> DomInheritTo;

}
== add {
	Rep ==> Record(tag: Int,
		       namer:  DomNamer,
		       noname:  Reserved,
                       getter: DomGetter,
		       hasher: DomHasher,
		       inheriter: DomInheritTo);

	import from Rep;

        axiomxlDispatchTag ==> 0;

	new(n: DomNamer, g: DomGetter, h: DomHasher, 
	    i: DomInheritTo): %
		== per [axiomxlDispatchTag, n, nil, g, h, i];

	tag(dv: %)   : Int	 == rep(dv).tag;
	reserved(dv: %): Reserved == rep(dv).noname;
	getter(dv: %): DomGetter == rep(dv).getter;
	hasher(dv: %): DomHasher == rep(dv).hasher;
	namer(dv: %) : DomNamer  == rep(dv).namer;
	inheriter(dv: %) : DomInheritTo  == rep(dv).inheriter;

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

+++ CatObj is the top-level category representation.
CatObj: Conditional with {
	new:		CatRep -> %;
		++ new(cr) creates a new category by wrapping
		++ a dispatch vector around a CatRep.

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

	makeDummy:	() -> %;
	fill!:		(%, %) -> ();
	reFill!:	(%, CatDispatchVector, CatRep) -> ();
}
== add {
	Rep ==> Record (cdv:	CatDispatchVector,
			catRep:	CatRep);

	import from Rep;
	import from PointerCatObj;

	catRep     (cat: %): CatRep		== rep(cat).catRep;
	dispatcher (cat: %): CatDispatchVector	== rep(cat).cdv;

	new (cdv: CatDispatchVector, crep: CatRep) : % ==
		per [cdv, crep];
	new (crep: CatRep) : % ==
		per [axiomxlCatDispatchVector(), crep];

	makeDummy(): % == per [ dummyDispatchVector(), Nil CatRep];
	makePointer(fn: ()-> %): % == 
			per [pointerCatDV(), new(fn) pretend CatRep];

	fill!(cat: %, val: %): () == {
		rep(cat).cdv 	:= dispatcher(val);
		rep(cat).catRep	:= catRep(val);
	}
	
	reFill!(cat: %, dv: CatDispatchVector, r: CatRep): () == {
		rep(cat).cdv 	:= dv;
		rep(cat).catRep	:= r;
	}
		
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

+++ CatRep defines the run-time representation of axiomxl categories.
CatRep: Conditional with {
	new:		(CatRepInit %, ()->Hash, ()->DomainName) -> %;
		++ new(fun) creates a new category.

	prepare!:	% -> ();
		++ prepare!(cat) forces a category to fully instantiate.

	addExports!:	(%, Array Hash, Array Hash, Array Value) -> ();
		++ addExports!(cat, names, types, exports)
		++ sets the exports fields of a category.

	addHashFn!:	(%, ()->Hash) -> ();
	addParents!:	(%, Array CatObj) -> ();
		++ addParents!(cat, pars) set the parent field of a category.
	addNameFn!: 	(%, ()->DomainName) -> ();
		++ addName! sets the name of an category.

	axiomxlCatDispatchVector: () -> CatDispatchVector;
		++ axiomxlCatDispatchVector() creates the dispatch vector
		++ for axiomxl categories.
	dummyDispatchVector: () -> CatDispatchVector;
		++ creates the dispatch vector for unfilled categories 
}
== add {
	--- this representation is a compromise between 
	--- non-instantiated categories and built categories.
	--- it is quite possible to split the two as a 
	--- non-instantiated category should never be asked for exports
	--- but could be asked for a hashcode.
	Rep1 ==> Ptr;
	Per1 ==> CatRepInit %;
	rep1 x ==> x @ Per1 pretend Rep1;
	per1 x ==> x @ Rep1 pretend Per1;

	Rep ==> Record (buildFn:	Rep1,
			hashFn:		()->Hash,
			dc:		PtrCache,
			domain:		Domain,
			parents:	Array CatObj,
			nparents:	Int,
			names:		Array Hash,
			types:		Array Hash,
			exports:	Array Value,
			nameFn:		()->DomainName);

	import from Rep, String;

	local serialThis: Int := 1;
	local get1;

	categoryStdName(): DomainName == noName();

	new(fn: Per1, hfn: ()->Hash, nFn: ()->DomainName): % == {
		import from PtrCache;
		per [rep1 fn, hfn, newCache(),
		     Nil Domain, Nil Array CatObj, 0,
		     Nil Array Hash, Nil Array Hash,
		     Nil Array Value, nFn ];
	}
	hashFn	(x: %): ()->Hash	== rep(x).hashFn;
	names	(x: %): Array Hash	== rep(x).names;
	types	(x: %): Array Hash	== rep(x).types;
	exports	(x: %): Array Value	== rep(x).exports;
	parents	(x: %): Array CatObj	== rep(x).parents;
	nparents(x: %): Int		== rep(x).nparents;
	dom	(x: %): Domain		== rep(x).domain;
	cache	(x: %): PtrCache	== rep(x).dc;
	nameFn	(x: %): ()->DomainName	== rep(x).nameFn;
	builder	(x: %): Per1		== per1(rep(x).buildFn);

	prepare! (cat: %): () ==
		if (Nil?(Array Hash)(names(cat))) then {
			builder(cat)(cat, dom(cat));
		}

	addExports!(cat: %, nams: Array Hash, typs: Array Hash,
		    expts: Array Value): () == {
		rep(cat).names    := nams;
		rep(cat).types    := typs;
		rep(cat).exports  := expts;
	}

	addParents!(cat: %, prnts: Array CatObj): () == {
		dom0 := dom(cat);
		i := 0;
		for p: CatObj in prnts repeat {
			if p then {
				i := i + 1;
				prnts.i := categoryBuild(p, dom0);
			}
		}
		rep(cat).parents  := prnts;
		rep(cat).nparents := i;
	}

	addHashFn!(cat: %, fn: ()->Hash): () == {
		rep(cat).hashFn := fn;
	}
	
	addNameFn!(cat: %, nmfn: ()->DomainName): () == {
		rep(cat).nameFn := nmfn;
	}

	-- Axiomxl category dispatch vector functions.	

	name0(cat: %): DomainName == {
		nameFn(cat)();
	}

	build0(cat: %, d: Domain): CatObj == {
		import from PtrCache;
		DEBUG(Nil?(PtrCache)(cache(cat)) => ERROR "oops");
		DEBUG(PRINT() << "(Building: ");
		key: BasicTuple := tuple(d);
		DEBUG(PRINT() << name0 cat
			      << " [" << getName d << "]");
		(v, flag) := getEntry(cache(cat), key);
		flag => {
			r   := v pretend CatObj;
			DEBUG(PRINT() << " cached)" << NL());
			r
		}
		newCat := new(builder(cat), hashFn(cat), nameFn(cat));
		rep(newCat).domain  := d;
		rep(newCat).dc      := Nil PtrCache;
		cobj := new(newCat pretend CatRep);
		DEBUG(PRINT() << " new)" << NL());
		addEntry(cache(cat), key, cobj pretend Ptr);
		cobj
	}

	get0(cat: %, pcent: Domain, name: Hash, type: Hash, box: Box): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		DEBUG(PRINT() << NL() << "(Get Cat: " << find name << " " << type);
		if (name = pcentPcentHash) then {
			type ~= hash0(cat) => { 	
				DEBUG(PRINT() << " " << name0 cat << " "); 
				DEBUG(PRINT() << "Failed)" << NL()); 
				return nullBox();
			}
			DEBUG(PRINT() << " Done)" << NL()); 
			setVal!(box, pcent pretend Value);
			return box;
		}
		val := get1(cat, pcent, name, type, box);
		DEBUG(PRINT() << " " << name0 cat << " [" << domainName(dom cat) << "])" << NL());
		val;
	}
	get1(cat: %, pcent: Domain, name: Hash, type: Hash, box: Box): Box == {
		prepare!(cat);
		for expName  in names(cat)
		for expType  in types(cat)
		for expValue in exports(cat)
		repeat {
			name=expName and (type=expType or type=wildcard) => {
				setVal!(box, expValue);
				return box;
			}
		}
		nullBox();
	}

	hash0(cat: %): Hash == hashFn(cat)();

	parentCount0(cat: %): Int == {
		prepare!(cat);
		nparents(cat);
	}

	parentGet(cat: %, i: Int): CatObj == {
		prepare!(cat);
		parents(cat).i;
	}

	nameDummy(cat: %): DomainName == noName();
	buildDummy(cat: %, dom: Domain): CatObj == 
		ERROR "build on incomplete category";
	
	getDummy(cat: %, pcent: Domain, name: Hash,
	         type: Hash, box: Box): Box == 
		ERROR "get on incomplete category";
	
	hashDummy(cat: %): Hash ==
		ERROR "hash on incomplete category";

	parentGetDummy(cat: %, i: Int): CatObj == 
		ERROR "parent get on incomplete category";

	parentCountDummy(cat: %): Int ==
		ERROR "parent count on incomplete category";

	-- Dispatch vector creation operations.

	--!! These functions are here rather than in CatDispatchVector so
	--!! we can initialize the functions used to construct the vector
	--!! using global constants instead of using calls to domainGetExport!.

	DV ==> CatDispatchVector;

	local dv: DV := Nil DV;
	local ddv: DV := Nil DV;

	axiomxlCatDispatchVector (): DV == {
		free dv: DV;
		not dv => dv := new(name0  pretend CatNamer,
				    build0 pretend CatBuilder,
				    get0   pretend CatGetter,
				    hash0  pretend CatHasher,
				    parentCount0 pretend CatParentCounter,
				    parentGet    pretend CatParentGetter);
		dv;
	}

	dummyDispatchVector (): DV == {
		free ddv: DV;
		not ddv => ddv := new(nameDummy  pretend CatNamer,
				    buildDummy pretend CatBuilder,
				    getDummy   pretend CatGetter,
				    hashDummy  pretend CatHasher,
				    parentCountDummy pretend CatParentCounter,
				    parentGetDummy   pretend CatParentGetter);
		ddv;
	}

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %): TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

PointerCatObj: with {
	pointerCatDV: () -> CatDispatchVector;
	new:	      (()->CatObj) -> CatRep;
} == add {
	Rep ==> Union(init: () -> CatObj, cat: CatObj);
	DV ==> CatDispatchVector;
	import from Rep;

	pcdv: CatDispatchVector := Nil(CatDispatchVector);

	new(fn: () -> CatObj): CatRep == [fn] pretend CatRep;

	deref(c: %): CatObj == {
		if rep(c) case init then {
			DEBUG(PRINT() << "(Deref");
			rep(c).cat := rep(c).init();
			DEBUG(PRINT() << rep(c).cat pretend SingleInteger <<")"<<newline);
		}
		rep(c).cat;
	}

	name0(c: %): DomainName == categoryName(deref c);
	-- maybe should be lazier
	build0(c: %, dom: Domain): CatObj == categoryBuild(deref c, dom);
	
	get0(c: %, pc: Domain, n1: Hash, n2: Hash, box: Box): Box == 
		getDefault!(deref c, pc, n1, n2, box);
	
	hash0(c: %): Hash == hash(deref c);

	parentCount0(c: %): Int		== parentCount(deref c);
	parentGet(c: %, n: Int): CatObj == getParent(deref c, n);

	pointerCatDV(): DV == {
		free pcdv: DV;
		not pcdv => pcdv := new(name0  pretend CatNamer,
				    build0 pretend CatBuilder,
				    get0   pretend CatGetter,
				    hash0  pretend CatHasher,
				    parentCount0 pretend CatParentCounter,
				    parentGet    pretend CatParentGetter);
		pcdv;
	}

}

+++ Structure containing a category's protocol for getting parents/hash codes.
CatDispatchVector: Conditional with {
	new:		(CatNamer, CatBuilder, CatGetter, CatHasher,
			 CatParentCounter, CatParentGetter) -> %;
		++ new(build, get, hash, parentCount, parentGet)
		++ constructs a category dispatch vector.

	builder:	% -> CatBuilder;
		++ builder(cat) returns the building function of the category.

	getter:		% -> CatGetter;
		++ getter(cat) returns the getter function of the category.

	hasher:		% -> CatHasher;
		++ hasher(cat) returns the hasher function of the category.
	namer:		% -> CatNamer;
		++ returns the naming function of the category.
	parentCounter:	% -> CatParentCounter;
		++ parentCounter(cat) returns the #parents function.

	parentGetter:	% -> CatParentGetter;
		++ parentGetter returns the getParent function.
}
== add {
	Rep ==> Record (tag:		Int,
			namer:		CatNamer,
			noname: 	Reserved,
			getter:		CatGetter,
			hasher:		CatHasher,
			builder:	CatBuilder,
			parentCounter:	CatParentCounter,
			parentGetter:	CatParentGetter);
			
	import from Rep;

	axiomxlCatDispatchTag ==> 10;

	new	( namer:	CatNamer,
		 builder:	CatBuilder,
		 getter:	CatGetter,
		 hasher:	CatHasher,
		 parentCounter:	CatParentCounter,
		 parentGetter:	CatParentGetter) : %

		== per[axiomxlCatDispatchTag, namer, nil, getter, hasher,
		       builder, parentCounter, parentGetter];

	builder	(cdv: %): CatBuilder		== rep(cdv).builder;
	reserved(cdv: %): Reserved		== rep(cdv).noname;
	getter	(cdv: %): CatGetter		== rep(cdv).getter;
	hasher	(cdv: %): CatHasher		== rep(cdv).hasher;
	parentCounter(cdv: %): CatParentCounter	== rep(cdv).parentCounter;
	parentGetter (cdv: %): CatParentGetter	== rep(cdv).parentGetter;
	namer	(cdv: %): CatNamer		== rep(cdv).namer;
	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %): TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

Box: Conditional with {
	new:		Value -> %;
		++ new(val) creates an initialized box.

	value:		% -> Value;
		++ value(box) returns the value from the box.

	setVal!:	(%, Value) -> ();
		++ setVal!(box, val) sets the value in a box.

	nullBox:	() -> %;
		++ nullBox() returns the distinguished empty box,
		++ which cannot hold a value.
}
== add {
	Rep ==> Record(value: Value);

	import from Rep;

	new	(v: Value): %		== per [v];
	value	(b: %): Value		== rep(b).value;
	setVal!	(b: %, v: Value): ()	== rep(b).value := v;
	nullBox	(): %			== Nil %;

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

LazyImport: with { 
	makeLazyImport: (Domain, Hash, Hash) -> %;
	force: %->Value;
} == add {
	--Need set!$Union, or for closures to work better under -Wruntime
	Rep ==> Record(got: Boolean,
		       dom: Domain, 
		       name: Hash, 
		       type: Hash, 
		       value: Value);
	import from Rep;
	makeLazyImport(dom: Domain, n: Hash, t: Hash): % == 
		per [false, dom, n, t, Nil Value];

	force(lv: %): Value == {
		if not rep(lv).got then {
			rep(lv).value := domainGetExport!(rep(lv).dom, 
							  rep(lv).name, 
							  rep(lv).type);
			rep(lv).got := true;
			rep(lv).dom := Nil Domain;
		}
		return rep(lv).value;
	}		
}

StringTable: with {
	addNames: (Array Hash, Array String)->();
	find: Hash->String;
} == add {
	import from List String, List Hash, Format;
	names: List String := empty();
	codes: List SingleInteger := empty();

	addNames(a1: Array Hash, a2: Array String):() == {
		free names, codes;
		if empty?(names) then {
			names := cons("%%", names);
			codes := cons(pcentPcentHash, codes);
		}
		for code in a1 
		for name in a2 repeat {
			codes := cons(code, codes);
			names := cons(name, names);
		}
	}

	find(i: Hash): String == {
		for code in codes
		for name in names repeat {
			if code = i then return name
		}	
		x: String := copy "??:             ";
		end!(x, format(i, x, 3)+1);
		x
	}
}


BasicTuple: BasicType with {
	length:   % -> SingleInteger;
	element: (%, SingleInteger) -> Ptr;
	tuple: Tuple Domain  -> %;
	tuple: Tuple Pointer -> %;
	dispose!:  % -> ();
} == add {
	Rep ==> Tuple Ptr;
	import from Rep, SingleInteger, Ptr;
	
	tuple(t: Tuple Domain):  % == t pretend BasicTuple;
	tuple(t: Tuple Pointer): % == t pretend BasicTuple;

        import { PtrMagicEQ: (Ptr,Ptr) -> Boolean }  from Builtin ;
	(a: %) = (b: %): Boolean == {
		(length a) ~= (length b) => false;
		for i in 1..length a repeat {
			if not PtrMagicEQ( element(a,i) , element(b,i)) then return false
		}
		return true;
	}
	(a: %) ~= (b: %): Boolean == not (a = b);
	(p: TextWriter) << (t: %): TextWriter == {
		p << "(tuple: " << length t << " ";
		for i in 1..length t repeat 
			p << "??" << " ";
		p << ")";
	}
	length(x: %): SingleInteger        == length(rep x);
	element(x: %, n: SingleInteger): Ptr == element(rep x, n);
	sample: % == Nil %;
	dispose!(x: %): () == dispose! rep(x);

	hash(x:%):SingleInteger ==
	{
		local result:SingleInteger := length(x);
		local elem:SingleInteger;

		for i in 1..length x repeat
		{
			elem   := element(x,i) pretend SingleInteger;
			result := xor(result, hash elem);
		}

		result;
	}


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}

PtrCache: with {
	addEntry: (%, BasicTuple, Ptr) -> Ptr;
	getEntry: (%, BasicTuple) -> (Ptr, Boolean);
	newCache: () -> %;
	newCache: SingleInteger -> %;
} == add {
	-- how big to make the cache (if unspecified)
	cacheSize ==> 15;
	Rep ==> Record(posn: SingleInteger, 
		       keys: Array BasicTuple, 
		       values: Array Ptr,
		       size: SingleInteger);
	import from Rep, SingleInteger, Array BasicTuple;
	
	keys(x: %): Array BasicTuple == rep(x).keys;
	values(x: %): Array Ptr == rep(x).values;
	posn(x: %): SingleInteger == rep(x).posn;
	size(x: %): SingleInteger == rep(x).size;

	newCache(): % == per([0, 
			     new(cacheSize, Nil(BasicTuple)), 
			     new(cacheSize, Nil Ptr),
			     cacheSize]);

	-- He who calls with (x < 1) is eternally doomed.
	newCache(x:SingleInteger): % == per([0, 
			     new(x, Nil(BasicTuple)), 
			     new(x, Nil Ptr),
			     x]);

	addEntry(cache: %, key:  BasicTuple, value: Ptr): Ptr == {
		--(val, flag) := getEntry(cache, key);
		--flag => val;
		DEBUG(PRINT()<<NL()<<"Adding: "<<cache pretend SingleInteger
		             <<key<<NL());
		keys(cache).(posn(cache)+1) := key;
		values(cache).(posn(cache)+1) := value;
		rep(cache).posn := mod_+(posn(cache), 1, size(cache));
		value
	}

	getEntry(cache: %, key: BasicTuple): (Ptr, Boolean) == {
		for k in keys(cache)
		for v in values(cache) repeat {
			not Nil?(BasicTuple) k and k = key => return(v, true);
		}
		(nil, false)
	}
}

#if DynamicCache
Key ==> BasicTuple;
CacheTable: BasicType with {
	table: () -> %;
		++ `table()' creates a new table using the equality test `='
		++ and the hash function `hash' from the `Key' type.

	#: % -> SingleInteger;
		++ `#t' returns the number of elements in `t'.

	search: (%, Key, Value) -> (Boolean, Value);
		++ `(b,v) := search(t,k,d)' searches table `t' for the value
		++ associated with key `k'. If there is such a value, `vk',
		++ then `b' is set to `true' and `v' is set to `vk'. 
		++ Otherwise `b' is `false' and `v' is set to `d'.

	set!: (%, Key, Value) -> Value;
		++ `t.k := val' associates `val' with `k' in `t'.

	drop!: (%, Key) -> Value;
		++ `drop!(t, k)' removes the entry for `k' in `t'.

	dispose!: % -> ();
		++ `dispose! t' indicates a table will no longer be used.
}
== add {
	-- Parameters to tune table performance.
	InitBuckC ==> primes.3;
	MaxLoad	  ==> 5.0;
	MinLoad   ==> 0.5;


	-- primes.i is the largest prime <= 2^i.
	local primes: Array SingleInteger == [
		2,         3,          7,          13,
		31,        61,         127,        251,
		509,       1021,       2039,       4093,
		8191,      16381,      32749,      65521,
		131071,    262139,     524287,     1048573,
		2097143,   4194301,    8388593,    16777213,
		33554393,  67108859,   134217689,  268435399,
		536870909, 1073741789, 2147483647, 4294967291
	];
	local lg(n: SingleInteger): SingleInteger == {
		p := 1;
		for i in 0.. repeat { if n <= p then return i; p := p + p; }
		never
	}

	-- Representation
	Entry ==> Record(key: Key, value: Value, hash: Hash);

	Rep   ==> Record(
			count:	SingleInteger,
			buckv:	Array List Entry);

	-- Local representation operaitons
	import from Rep;

	local new(): % ==
		per [0, new(InitBuckC, nil)];

	local buckv(t: %):Array List Entry	 == rep(t).buckv;
	local buckc(t: %):SingleInteger	 == #rep(t).buckv;

	local inc!(t: %): () == {
		import from SingleFloat;
		rep(t).count := rep(t).count + 1;
		if #t::SingleFloat/buckc(t)::SingleFloat > MaxLoad then
			enlarge! t;
	}
	local dec!(t: %): () == {
		import from SingleFloat;
		rep(t).count := rep(t).count - 1;
		if #t::SingleFloat/buckc(t)::SingleFloat < MinLoad then
			shrink! t;
	}
		
	-- Find the chain for k, moving the link to the front on success.
	local findChain(t: %, k: Key): SingleInteger == {
		h := hash(k);
		n := h mod buckc(t) + 1;
		b := buckv(t).n;
		p := nil@List Entry;	-- Previous link or nil.

		while not empty? b repeat {
			e := first b;
			if h = e.hash then {
				if (e.key = k) then {
					-- Move to front
					if not empty? p then {
						p.rest     := b.rest;
						b.rest     := buckv(t).n;
						buckv(t).n := b;
					}
					return n;
				}
			}
			p := b;
			b := rest b;
		}
		return 0;
	}

	-- Resize the table, larger or smaller.
	local enlarge!(t: %): % == resize!(t, lg buckc(t) + 1);
	local shrink! (t: %): % == resize!(t, lg buckc(t) - 1);

	local resize!(t: %, sizeix: SingleInteger): % == {
		sizeix < 1 or sizeix > #primes => t;

		nbuckc := primes sizeix;
		nbuckv := new(nbuckc, nil);

		for b0 in buckv t repeat {
			b := b0;
			while b repeat {
				hd := b;
				b  := b.rest;

				n  := (hd.first.hash mod nbuckc) + 1;
				hd.rest  := nbuckv.n;
				nbuckv.n := hd;
			}
		}
		dispose! rep(t).buckv;
		rep(t).buckv := nbuckv;
		t;
	}


	-- Exported operations
	sample: % == table();

	(t1: %) = (t2: %): Boolean == {
      import { PtrMagicEQ: (Ptr,Ptr) -> Boolean }  from Builtin ;
		PtrMagicEQ(t1 pretend Ptr, t2 pretend Ptr);
	}

	(out: TextWriter) << (t: %): TextWriter == out;

	#(t: %): SingleInteger == rep(t).count;

	table(): %   == new();

	search(t: %, k: Key, def: Value): (Boolean, Value) == {
		local lis:List Entry;
		n := findChain(t, k);
		if n = 0 then
			(false, def)
		else
		{
			lis := buckv(t).n;
			if (empty? lis) then
				(false, def); -- Impossible but kills test in first$List().
			else
				(true, lis.first.value)
		}
	}
	set!(t: %, k: Key, v: Value) : Value == {
		local lis:List Entry;
		n := findChain(t, k);
		n > 0 =>
		{
			lis := buckv(t).n;
			if not(empty? lis) then -- Lifts test from first$List().
				lis.first.value := v;
			v;
		}
		h := hash(k);
		n := (h mod buckc(t)) + 1;
		buckv(t).n := cons([k,v,h], buckv(t).n);
		inc! t;
		v;
	}
	drop!(t: %, k: Key): Value == {
		n := findChain(t, k);
		n = 0 => error "Element missing from table.";
		e := buckv(t).n.first;
		v := e.value;
		buckv(t).n := disposeHead! buckv(t).n; -- Dispose of the link.
		dispose! e;                            -- Dispose of the record.
		dec! t;
		v;
	}

	dispose!(t: %): () == {
		for b in buckv(t) repeat dispose! b;
		dispose! buckv(t);
		dispose! rep(t);
	}


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}

VariablePtrCache: with {
	addEntry: (%, BasicTuple, Ptr) -> Ptr;
	getEntry: (%, BasicTuple) -> (Ptr, Boolean);
	newCache: () -> %;
} == add {
	Rep == CacheTable;
	import from Rep, SingleInteger, BasicTuple;
	
	newCache(): % == per table();

	addEntry(cache: %, key:  BasicTuple, value: Ptr): Ptr == {
		DEBUG(PRINT()<<"Adding: "<<cache pretend SingleInteger <<key<<NL());
		(rep cache).key := (value pretend Value);
		value
	}

	getEntry(cache: %, key: BasicTuple): (Ptr, Boolean) == {
		(found, result) := search(rep cache, key, (nil@Ptr) pretend Value);
		(result pretend Ptr, found);
	}
}
#endif

checkGetCache(cache: PtrCache, pcent: Domain, 
	      name: Hash, type: Hash, skip: Bit): 
			(Box, Bit, BasicTuple) == {
	import from BasicTuple;
	key: BasicTuple := tuple(pcent pretend Ptr, name pretend Ptr, 
	  			 type pretend Ptr,  skip pretend Ptr);
	(val, flag) := getEntry(cache, key);
	DEBUG(if flag then PRINT() << "Found in cache" << NL());
	if flag then {
		dispose! key;
		key := Nil BasicTuple;
	}
	(val pretend Box, flag, key)
}

addGetCache(cache: PtrCache, key: BasicTuple, box: Box): () == {
	-- must copy the box...
	val := if not box then Nil Ptr else (new value box) pretend Ptr;
	addEntry(cache, key, val);
}


domainMake(df: DomainFun DomainRep): Domain ==
	new(new(df)$DomainRep);

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

domainTestExport!(td: Domain, name: Hash, type: Hash): Bit == 
	testExport!(td, name, type);
	
domainHash!(td: Domain): Hash ==
	getHash!(td);

domainName(td: Domain): DomainName ==
	getName(td);

domainMakeDummy(): Domain == makeDummy();

domainFill!(d: Domain, v: Domain): () == fill!(d, v);
	
categoryMake(fn: CatRepInit CatRep, 
	     hfn: ()->Hash, 
             nameFn: ()-> DomainName): CatObj == 
	new(new(fn, hfn, nameFn)$CatRep);

categoryAddParents!(c: CatRep, parents: Array CatObj, cAsCat: CatObj): () ==
	addParents!(c, parents);

categoryAddExports!(c: CatRep, names: Array Hash, types: Array Hash,
		  exports: Array Value): () ==
	addExports!(c, names, types, exports);

categoryAddNameFn!(cat: CatRep, name: ()->DomainName): () ==
	addNameFn!(cat, name);

categoryBuild(cat: CatObj, dom: Domain): CatObj ==
	build(cat, dom);

categoryName(cat: CatObj): DomainName ==
	name(cat);

categoryMakeDummy(): CatObj == makeDummy();

categoryFill!(d: CatObj, v: CatObj): () == fill!(d, v);

domainPrepare!(td: Domain): () == {
	import from DomainRep;
	prepare! domainRep td;
}

lazyGetExport!(dom: Domain, n: Hash, t: Hash): LazyImport ==
	makeLazyImport(dom, n, t);

lazyForceImport(li: LazyImport): Value == force(li);

noOperation():() == ();

extendMake(df: DomainFun DomainRep): Domain == {
	dom := newExtend(new(df)$DomainRep);
#if ExtendReplace
	getHash! dom;
#endif
	dom;
}

extendFill!(dom: DomainRep, pars: Array Domain): () ==
	extendFillObj!(dom, pars);

rtConstSIntFn(x: SingleInteger)(): SingleInteger == x;

stringConcat(t: Tuple String): String == { import from String; concat t }

rtAddStrings(a1:Array Hash, a2: Array String): () == { 
	import from StringTable; 
	addNames(a1, a2);
}

-- cache manipulation functions
export {
	rtCacheMake:  () -> PtrCache;
	rtCacheExplicitMake:  BSInt -> PtrCache;
	rtCacheCheck: (PtrCache, Tuple Ptr) -> (Ptr, Boolean);
	rtCacheAdd:   (PtrCache, Tuple Ptr, Ptr) -> Ptr;
} to Foreign(Builtin);

import from PtrCache;

rtCacheMake(): PtrCache == newCache();

rtCacheExplicitMake(x:BSInt): PtrCache == newCache(x::SingleInteger);

rtCacheCheck(cache: PtrCache, key: Tuple Ptr): (Ptr, Boolean) == 	
		getEntry(cache, key pretend BasicTuple);

rtCacheAdd(cache: PtrCache, key: Tuple Ptr, value: Ptr): Ptr == 
		addEntry(cache, key pretend BasicTuple, value);


#if DynamicCache
-- dynamic cache manipulation functions
export {
	rtCacheDynamicMake:  () -> VariablePtrCache;
	rtCacheDynamicCheck: (VariablePtrCache, Tuple Ptr) -> (Ptr, Boolean);
	rtCacheDynamicAdd:   (VariablePtrCache, Tuple Ptr, Ptr) -> Ptr;
} to Foreign(Builtin);

import from VariablePtrCache;

rtCacheDynamicMake(): VariablePtrCache == newCache();

rtCacheDynamicCheck(cache: VariablePtrCache, key: Tuple Ptr):(Ptr, Boolean) ==
		getEntry(cache, key pretend BasicTuple);

rtCacheDynamicAdd(cache: VariablePtrCache, key: Tuple Ptr, value: Ptr): Ptr == 
		addEntry(cache, key pretend BasicTuple, value);
#endif

InitFn ==> SingleInteger -> Value;

export {
	rtDelayedInit!: (InitFn, SingleInteger) -> (()->Value);
	rtDelayedGetExport!: (Domain, Hash, Hash) -> (()->Value);
} to Foreign(Builtin);

rtDelayedInit!(fn: InitFn, n: SingleInteger)(): Value == 
	fn n;

rtDelayedGetExport!(d: Domain, n1: Hash, n2: Hash)(): Value ==
	domainGetExport!(d, n1, n2);


export {
	rtLazyCatFrInit: (InitFn, SingleInteger) -> CatObj;
	rtLazyDomFrInit: (InitFn, SingleInteger) -> Domain;
} to Foreign(Builtin);

rtLazyCatFrInit(fn: InitFn, n: SingleInteger): CatObj == {
	ifn(): CatObj == fn(n) pretend CatObj;
	rtLazyCatFrFn(ifn);
}

rtLazyCatFrFn(fn: ()->CatObj): CatObj == {
	import from PointerCatObj, CatObj;
	cat: CatObj := categoryMakeDummy();
	reFill!(cat, pointerCatDV(), new fn);
	cat
}

rtLazyDomFrInit(fn: InitFn, n: SingleInteger): Domain == {
	ifn(): Domain == fn(n) pretend Domain;
	rtLazyDomFrFn(ifn);
}

rtLazyDomFrFn(fn: () -> Domain): Domain == {
	import from PointerDomain;
	dom := domainMakeDummy();
	reFill!(dom, pointerDV(), new fn);
	dom
}
export {
	namePartConcat:   (Boolean, Tuple DomainName) -> DomainName;
	namePartFrOther:  Pointer	   -> DomainName;
	namePartFrString: String	   -> DomainName;
	rtConstNameFn: String -> (() -> DomainName);
	rtSingleParamNameFn: (String, Domain) -> (() -> DomainName);
--	rtSpecialNameFn: (dn: DomainName, T: Tuple Domain) -> (() -> DomainName);
} to Foreign Builtin;

namePartConcat(x: Boolean, T: Tuple DomainName): DomainName == {
	import from List DomainName;
	combine(x, [T]);
}

namePartFrString(s: String): DomainName == new s;

namePartFrOther(p: Pointer): DomainName == other p;

rtConstNameFn(s: String)(): DomainName == new s;

rtSingleParamNameFn(s: String, d: Domain)(): DomainName == 
	combine(false, (new s, domainName d));

----------------------------------------------------------------------------
-- This unmitigated hack needed cos import from foreign builtin don't work

import {
	fiSetDebugVar: Value -> ();
	fiGetDebugVar: () -> Value;
} from Foreign;

export {
	rtDoDebug:    (Int, String, Int, Value, Value, Value) -> ();
	--rtSetDebugFn: ((Int, String, Int, Value, Value, Value) -> ())-> ();
} to Foreign Builtin;

fiSetDebugVar(
   ((a: Int, s: String, i: Int, v1: Value, v2: Value, v3: Value): () 
						+-> return)
			pretend Value);

rtDoDebug(a: Int, s: String, i: Int, v1: Value, v2: Value, v3: Value): () == {
	(fiGetDebugVar() pretend ((Int, String, Int, Value, Value, Value) -> ()))(a,s,i,v1,v2, v3);
}

----------------------------------------------------------------------------
-- This is an attempt at improving the original debugging hooks. Eventually
-- these will replace the ones above (which aren't actually used anywhere).
-- 
-- Programs compiled with -Wdebugger have debug-event calls to rtDebug*()
-- described by the category below.

#if 0
define RuntimeDebugHandlers:Category == with
{
	rtDebugEnter: (String, Int, Domain, String, Domain, Int) -> Ptr;
		++ `rtDebugEnter(fil, lin, exporter, fun, type, argc)'
		++ is invoked when the function `fun' begins executing.
		++ The function is defined at line `lin' in file `fil'
		++ with return type `type' and having `argc' arguments.
		++ The return value is an identifier unique to this call
		++ instance and is used to provide a context for other
		++ debug events (such as rtDebugReturn).

	rtDebugInit: (String, Int, Domain, String, Domain, Int) -> Ptr;
		++ `rtDebugInit(fil, lin, exporter, fun, type, argc)'
		++ is invoked when the domain constructor `fun' begins
		++ executing. The constructor is defined at line `lin'
		++ in file `fil' with type `type' and `argc' arguments.
		++ The return value is the same as rtDebugEnter.

	rtDebugInside: Ptr -> ();
		++ `rtDebugInside(fun)' is invoked when the context
		++ `fun' has now been entered. This only occurs after
		++ a function entry event and the assignment events
		++ for each parameter have completed. Once this event
		++ has been received the full details of the function
		++ and its parameters can be displayed.

	rtDebugReturn: (String, Int, Ptr, Domain, Value) -> ();
		++ `rtDebugReturn(fil, lin, fun, type, value)' is
		++ invoked when the function represented by the context
		++ `fun' (obtained from rtDebugEnter or rtDebugInit)
		++ returns a value `value:Type'.

	rtDebugExit: (String, Int, Ptr) -> ();
		++ `rtDebugExit(fil, lin, fun)' is invoked when the
		++ context `fun' exits with no return value.

	rtDebugAssign: (String, Int, Ptr, String, Domain, Value, Int, Int) -> ();
		++ `rtDebugAssign(fil, lin, fun, name, type, val, depth, no)'
		++ is invoked when the identifier `name' is assigned the
		++ value `val:type' at line `lin' in file `fil' while
		++ executing in the context `fun'. The lexical depth of
		++ the identifier is `depth' with negative values used
		++ to represent the following special cases:
		++    -1 : local
		++    -2 : parameter to `fun'
		++    -3 : parameter to the next call (see rtDebugCall)
		++    -4 : global
		++    -5 : fluid
		++ The value of `no' is the identifier number used by the
		++ Loc, Par, Glo and Lex FOAM values.

	rtDebugThrow: (String, Int, Ptr, Domain) -> ();
		++ `rtDebugThrow(fil, lin, fun, exn)' is invoked when the
		++ exception `exn' is thrown in the context `fun'.

	rtDebugCatch: (String, Int, Ptr, Domain) -> ();
		++ `rtDebugCatch(fil, lin, fun, exn)' is invoked when the
		++ exception `exn' is caught in the context `fun'.

	rtDebugCall: (String, Int, Ptr, String, Domain, Domain, Int) -> ();
		++ `rtDebugCall(fil, lin, fun, name, exporter, type, argc)'
		++ is invoked when the function called `name' is called at
		++ line `lin' of file `fil' in the context `fun'. If the
		++ function is an export then `exporter' is the domain
		++ which exported it. The type of the function is `type'
		++ and it has `argc' arguments. The values of these
		++ arguments were passed to rtDebugAssign() immediately
		++ prior to this event.

	rtDebugStep: (String, Int, Ptr) -> ();
		++ `rtDebugStep(fil, lin, fun)' is invoked when the program
		++ is about to execute the statement at line `lin' in file
		++ `fil' in context `fun'. Used for single-stepping through
		++ code and for breakpoint handling.
}
#endif


-- Export the different types of debug event (see foam_c.h): note
-- that enumerations aren't available because that would require
-- a get from langx.
DbgInit		==> (0@Int);
DbgEnter	==> (1@Int);
DbgInside	==> (2@Int);
DbgReturn	==> (3@Int);
DbgExit		==> (4@Int);
DbgAssign	==> (5@Int);
DbgThrow	==> (6@Int);
DbgCatch	==> (7@Int);
DbgStep		==> (8@Int);
DbgCall		==> (9@Int);


-- The runtime C library foam_c.c provides us with two functions
-- for interfacing with the user-defined debugger.
import
{
	fiSetDebugger: (Int, Value) -> ();
		++ Used to register a debugger call-back function.

	fiGetDebugger: Int -> Value;
		++ Used to retrieve the debugger call-back function.
} from Foreign;


-- Macros to make the handler signatures more compact.
DbgInitSIG   ==> ((String, Int, Domain, String, Domain, Int) -> Ptr);
DbgEnterSIG  ==> ((String, Int, Domain, String, Domain, Int) -> Ptr);
DbgInsideSIG ==> (Ptr -> ());
DbgReturnSIG ==> ((String, Int, Ptr, Domain, Value) -> ());
DbgExitSIG   ==> ((String, Int, Ptr) -> ());
DbgAssignSIG ==> ((String, Int, Ptr, String, Domain, Value, Int, Int) -> ());
DbgThrowSIG  ==> ((String, Int, Ptr, Domain) -> ());
DbgCatchSIG  ==> ((String, Int, Ptr, Domain) -> ());
DbgCallSIG   ==> ((String, Int, Ptr, String, Domain, Domain, Int) -> ());
DbgStepSIG   ==> ((String, Int, Ptr) -> ());


-- Export our hooks so that clients can access them.
export
{
	rtDebugInit:    DbgInitSIG;
	rtDebugEnter:   DbgEnterSIG;
	rtDebugInside:  DbgInsideSIG;
	rtDebugReturn:  DbgReturnSIG;
	rtDebugExit:    DbgExitSIG;
	rtDebugAssign:  DbgAssignSIG;
	rtDebugThrow:   DbgThrowSIG;
	rtDebugCatch:   DbgCatchSIG;
	rtDebugCall:    DbgCallSIG;
	rtDebugStep:    DbgStepSIG;
	rtDebugPaused?: () -> Boolean;
	rtDebugPaused!: Boolean -> Boolean;
} to Foreign Builtin;


-- Allow the compiler to enable/disable debugging.
local dbgPaused?:Boolean := false;


-- Return true if the debugger has been paused by the compiler.
rtDebugPaused?():Boolean ==
{
	free dbgPaused?:Boolean;
	return dbgPaused?;
}


-- Allow the compiler to pause the debugger, returning the old state.
rtDebugPaused!(state:Boolean):Boolean ==
{
	free dbgPaused?:Boolean;
	local result:Boolean;
	result := dbgPaused?;
	dbPaused? := state;
	return result;
}


-- These are the runtime debugger hooks that we export to Builtin and
-- which are invoked by code generated by the compiler with -Wdebugger.
-- We simply pass the details to the installed debugger. Note that it
-- only makes sense if all events are handled by the same debug system.
rtDebugInit(f:String, l:Int, e:Domain, n:String, t:Domain, c:Int):Ptr ==
{
	rtDebugPaused?() => return nil;
	((fiGetDebugger DbgInit) pretend DbgInitSIG)(f, l, e, n, t, c);
}

rtDebugEnter(f:String, l:Int, e:Domain, n:String, t:Domain, c:Int):Ptr ==
{
	rtDebugPaused?() => return nil;
	((fiGetDebugger DbgEnter) pretend DbgEnterSIG)(f, l, e, n, t, c);
}

rtDebugInside(x:Ptr):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgInside) pretend DbgInsideSIG)(x);
}

rtDebugReturn(f:String, l:Int, x:Ptr, t:Domain, v:Value):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgReturn) pretend DbgReturnSIG)(f, l, x, t, v);
}

rtDebugExit(f:String, l:Int, x:Ptr):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgExit) pretend DbgExitSIG)(f, l, x);
}

rtDebugAssign(f:String, l:Int, x:Ptr, n:String, t:Domain, v:Value, d:Int, c:Int):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgAssign) pretend DbgAssignSIG)(f, l, x, n, t, v, d, c);
}

rtDebugThrow(f:String, l:Int, x:Ptr, e:Domain):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgThrow) pretend DbgThrowSIG)(f, l, x, e);
}

rtDebugCatch(f:String, l:Int, x:Ptr, e:Domain):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgCatch) pretend DbgCatchSIG)(f, l, x, e);
}

rtDebugCall(f:String, l:Int, x:Ptr, n:String, e:Domain, t:Domain, c:Int):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgCall) pretend DbgCallSIG)(f, l, x, n, e, t, c);
}

rtDebugStep(f:String, l:Int, x:Ptr):() ==
{
	rtDebugPaused?() => return;
	((fiGetDebugger DbgStep) pretend DbgStepSIG)(f, l, x);
}


-- Local dummy call-backs for the default debugging system.
local dEnter, dInit, dReturn, dExit, dAssign, dThrow, dCatch, dCall, dStep;
dInit(f:String, l:Int, e:Domain, n:String, t:Domain, c:Int):Ptr == nil;
dEnter(f:String, l:Int, e:Domain, n:String, t:Domain, c:Int):Ptr == nil;
dInside(x:Ptr):() == {}
dReturn(f:String, l:Int, x:Ptr, t:Domain, v:Value):() == {}
dExit(f:String, l:Int, x:Ptr):() == {}
dAssign(f:String, l:Int, x:Ptr, n:String, t:Domain, v:Value, d:Int, c:Int):() == {}
dThrow(f:String, l:Int, x:Ptr, e:Domain):() == {}
dCatch(f:String, l:Int, x:Ptr, e:Domain):() == {}
dCall(f:String, l:Int, x:Ptr, n:String, e:Domain, t:Domain, c:Int):() == {}
dStep(f:String, l:Int, x:Ptr):() == {}


-- Install the dummy call-back functions.
fiSetDebugger(DbgInit, dInit pretend Value);
fiSetDebugger(DbgEnter, dEnter pretend Value);
fiSetDebugger(DbgInside, dInside pretend Value);
fiSetDebugger(DbgReturn, dReturn pretend Value);
fiSetDebugger(DbgExit, dExit pretend Value);
fiSetDebugger(DbgAssign, dAssign pretend Value);
fiSetDebugger(DbgThrow, dThrow pretend Value);
fiSetDebugger(DbgCatch, dCatch pretend Value);
fiSetDebugger(DbgCall, dCall pretend Value);
fiSetDebugger(DbgStep, dStep pretend Value);

----------------------------------------------------------------------------

export {
	stringHash: String -> SingleInteger;
} from Foreign Builtin;

import {
	fiStrHash: String -> SingleInteger
} from Foreign;

stringHash(s: String): SingleInteger == fiStrHash s;

export {
	rtAssertMessage: (String, SingleInteger, String) -> ();
} to Foreign Builtin;

import {
	fiRaiseException: String -> ();
} from Foreign;

rtAssertMessage(file: String, line: SingleInteger, text: String): () == {
	import { formatSInt: BSInt -> String } from Foreign;
	fiRaiseException concat("Assert failed in ", file ,", at line:", 
				  formatSInt(line::BSInt) , 
				 ": ", text)
}


#if DebugGets
-- To print a domain do: CFxxx_dbgPrint(0, Domain)
dbgPrint(d: Domain): () == {
	name := domainName(d);
	PRINT() << name << NL();
}
#endif
