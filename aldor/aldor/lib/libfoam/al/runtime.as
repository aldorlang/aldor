#include "foamlib"
#include "runtimelib.as"

InitFn ==> SingleInteger -> Value;
DV ==> DispatchVector;
CDV ==> CatDispatchVector;

WRAPP(txt, x) ==> { 
	  x
--	  PRINT() << "(" << txt << NL(); 
--	  wtmp := x; 
--	  PRINT() << " " << txt << " = " << wtmp << ")" << NL(); 
--	  wtmp
}

WRAP(txt, x) ==> { 
          x
--	  PRINT() << "(" << txt << NL(); 
--	  wtmp := x; 
--	  PRINT() << " " << txt << ")" << NL(); 
--	  wtmp
}

-- another hack: The recursive call implies that
-- it cannot be inlined from langx 
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

local printDomain(out: TextWriter, nm: DomainName): TextWriter == {
	fn(f: TextWriter, dom: DomainName): TextWriter == printDomain(fn, f, dom);
	printDomain(fn, out, nm);
}

local (<<)(out: TextWriter, nm: DomainName): TextWriter == printDomain(out, nm);

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

trace(s: String, T: BasicType, f: () -> T): T == { 
	 PRINT() << "{" << s << ":" << NL(); 
	 v := f(); 
	 PRINT() << " " << v << "}" << NL();
	 v
}

export {
	-- Functions for creating and enriching axiomxl domains.

	domainMake:		DomainFun(AldorDomainRep) -> Domain;
		++ domainMake(fun) creates a new lazy domain object.

	domainMakeDispatch:	AldorDomainRep -> Domain;
		++ domainMakeDispatch(dr) wraps a dispatch vector
		++ around a DomainRep.

	domainAddExports!:	(AldorDomainRep,
				 Array Hash, Array Hash, Array Value) -> ();
		++ domainAddExports!(dom, names, types, exports)
		++ Set the exports of a domain.

	domainAddDefaults!:	(AldorDomainRep, CatObj, Domain) -> ();
		++ domainAddDefaults!(dom, defaults, dom)
		++ Sets the default package for a domain.

	domainAddParents!:	(AldorDomainRep, Array Domain, Domain) -> ();
		++ defaultsAddExports!(dom, parents)
		++ Set the parents of a default package.

	domainAddHash!:		(AldorDomainRep, Hash) -> ();
		++ domainAddHash!(dom, hash) sets the hash code of a domain.

	domainAddNameFn!: 	(AldorDomainRep, ()->DomainName)->();
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

	categoryAddParents!:	(AldorCatRep, Array CatObj, CatObj) -> ();
		++ categoryAddExports!(dom, parents, self)
		++ Set the parents of a default package.
		++ additional arg is for uniformity
	categoryAddNameFn!: (AldorCatRep, ()->DomainName) -> ();
		++ Sets the name of a category.

	categoryAddExports!:	(AldorCatRep,
				 Array Hash, Array Hash, Array Value) -> ();
		++ categoryAddExports!(dom, names, types, exports)
		++ Set the exports of a category.

	categoryMake:		(CatRepInit(AldorCatRep), ()->Hash, ()->DomainName) -> CatObj;
		++ Constructing new cats

	categoryBuild:		(CatObj, Domain) -> CatObj;

	categoryName:		CatObj -> DomainName;
		++ Returns the name of a category
	categoryMakeDummy:		() -> CatObj;
	categoryFill!:		(CatObj, CatObj) -> ();

	extendMake:		DomainFun(AldorDomainRep) -> Domain;
		++ extendMake(fun) creates a new lazy extend domain object;
	extendFill!:	(AldorDomainRep, Array Domain) -> ();
		++ adds the extendee, extender pair to an extension domain

	noOperation:		() -> ();
		++ Do nothing --- used to clobber initialisation fns.

	-- Utility functions called from code generation.
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
#if NEVER



#endif
} to Foreign(Builtin);

domainMake(df: DomainFun AldorDomainRep): Domain == {
	arep: AldorDomainRep := newAldorDomainRep(df);
	domainMakeDispatch(arep);
}

domainMakeDispatch(dr: AldorDomainRep): Domain == 
	makeDomain(aldorDispatchVector(), dr pretend DomainRep);


domainMakeDummy(): Domain == {
        import from AldorDomainRep;
	makeDomain(dummyDispatchVector(), Nil DomainRep);
}

newAldorDomainRep(df: DomainFun AldorDomainRep): AldorDomainRep == {
    new(df);
}

extendMakeDispatch(dr: AldorDomainRep): Domain == {
	makeDomain(extendDispatchVector(), dr pretend DomainRep);
}

domainAddExports!(d: AldorDomainRep, names: Array Hash, types: Array Hash,
		  exports: Array Value): () == {
	addExports!(d pretend AldorDomainRep, names, types, exports);
}

local box: Box := new Nil Value;

domainGetExport!(td: Domain, name: Hash, type: Hash): Value == {
--	PRINT() << "(GET: " << domainName(td) << " " << find(theStringTable, name) << " " << type;
	v := domainGetExport1!(td, name, type);
--	PRINT() << ")"<<NL();
	v
}
domainGetExport1!(td: Domain, name: Hash, type: Hash): Value == {
--	PROFILE(import from StringTable);
	mybox := getExport0!(td, name, type, box);
	mybox => value mybox;
	failmsg(td, name, type);
	ERROR "Export not found";
--	PROFILE(PRINT()<< domainName td << " " << find nameCode
--			     <<" "  <<type<<NL());
	never;
}

local ERROR(s: String): Exit == {
	fiRaiseException s;	
	never;
}

local failmsg(td: Domain, name: Hash, type: Hash): () == {
	import from StringTable;
	PRINT() << "Looking in ";
	printDomain(PRINT(), getName td)
               	<< " for "       << find(theStringTable, name)
                << " with code " << type << NL();
}		

domainTestExport!(td: Domain, name: Hash, type: Hash): Bit == 
	testExport!(td, name, type);


domainAddDefaults!(d: AldorDomainRep, defaults: CatObj, d2: Domain): () ==
	addDefaults!(d, defaults, d2);


domainAddParents!(d: AldorDomainRep, parents: Array Domain, dAsDom: Domain): () ==
	addParents!(d, parents, dAsDom);

domainAddHash!(d: AldorDomainRep, hash: Hash): () ==
	addHash!(d, hash);

domainAddNameFn!(d: AldorDomainRep, namefn: ()->DomainName): () ==
	addNameFn!(d, namefn);

domainHash!(td: Domain): Hash ==
	getHash!(td);

domainName(td: Domain): DomainName ==
	getName(td);

domainFill!(d: Domain, v: Domain): () == fill!(d, v);

export {
	rtDelayedInit!: (InitFn, SingleInteger) -> (()->Value);
	rtDelayedGetExport!: (Domain, Hash, Hash) -> (()->Value);
} to Foreign(Builtin);

rtDelayedInit!(fn: InitFn, n: SingleInteger)(): Value == fn n;

rtDelayedGetExport!(d: Domain, n1: Hash, n2: Hash): () -> Value == {
     import from Pointer;
     if (d pretend Pointer = nil()) then never;
     (): Value +-> {
	domainGetExport!(d, n1, n2);
     }
}

--
-- :: Categories
--

categoryMake(fn: CatRepInit AldorCatRep, 
	     hfn: ()->Hash, 
             nameFn: ()-> DomainName): CatObj == {
	import from AldorCatRep;
	--r: AldorCatRep := new(fn, hfn, nameFn)$AldorCatRep;
	new(aldorCatDispatchVector(), new(fn, hfn, nameFn) pretend CatRep);
}

categoryMakeDummy(): CatObj == {
    import from AldorCatRep;
    new(dummyCatDispatchVector(), Nil CatRep); -- XX: Use a real dummy value
}

categoryAddParents!(cat: AldorCatRep, parents: Array CatObj, cAsCat: CatObj): () == {
    addParents!(cat, parents);
}

categoryAddNameFn!(cat: AldorCatRep, namefn: () -> DomainName): () == {
    addNameFn!(cat, namefn);
}

categoryAddExports!(cat: AldorCatRep, 
	            names: Array Hash, types: Array Hash, vals: Array Value): () == {
    addExports!(cat, names, types, vals);
}

categoryBuild(cat: CatObj, dom: Domain): CatObj == {
	build(cat, dom);
}

categoryName(cat: CatObj): DomainName == {
    name(cat);
}

categoryFill!(d: CatObj, v: CatObj): () == {
    fill!(d, v);
}

--
-- :: Extends
--

extendMake(df: DomainFun AldorDomainRep): Domain == {
        import from AldorDomainRep;
	dom := makeDomain(extendDispatchVector(), new df pretend DomainRep);
#if ExtendReplace
	getHash! dom;
#endif
	dom;
}

extendFill!(dom: AldorDomainRep, pars: Array Domain): () ==
	extendFillObj!(dom, pars);

--
-- ::Nooperation
--
noOperation():() == ();



--
-- :: Lazy domains
--

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

--
-- :: Lazy gets etc
--

lazyGetExport!(dom: Domain, n: Hash, t: Hash): LazyImport ==
	makeLazyImport(dom, n, t);

lazyForceImport(li: LazyImport): Value == force(li);

domainPrepare!(td: Domain): () == {
	import from AldorDomainRep;
	prepare!(domainRep td pretend AldorDomainRep);
}

rtConstSIntFn(x: SingleInteger)(): SingleInteger == x;
rtConstNameFn(s: String)(): DomainName == new s;
rtSingleParamNameFn(s: String, d: Domain)(): DomainName == 
	combine(false, (new s, domainName d));

theStringTable: StringTable := new();

rtAddStrings(a1:Array Hash, a2: Array String): () == { 
	import from StringTable; 
	addNames(theStringTable, a1, a2);
}

export {
	rtCacheMake:  () -> PtrCache;
	rtCacheExplicitMake:  BSInt -> PtrCache;
	rtCacheCheck: (PtrCache, Tuple Ptr) -> (Ptr, Boolean);
	rtCacheAdd:   (PtrCache, Tuple Ptr, Ptr) -> Ptr;
} to Foreign(Builtin);

import from PtrCache;

rtCacheMake(): PtrCache == newCache();

rtCacheExplicitMake(x:BSInt): PtrCache == {
    import from SingleInteger;			     
    newCache(x::SingleInteger);
}

rtCacheCheck(cache: PtrCache, key: Tuple Ptr): (Ptr, Boolean) == {	
--		PRINT() << "(Cache check";
		(pp, flg) := getEntry(cache, key pretend BasicTuple);
--		PRINT() << flg << " " << pp << ")" << NL();
		(pp, flg)
}
rtCacheAdd(cache: PtrCache, key: Tuple Ptr, value: Ptr): Ptr == 
		addEntry(cache, key pretend BasicTuple, value);




--
-- :: Domain Dispatch vectors
--
aldorDispatchVector() :  DV == {
	-- free adv;
	--not adv => adv := 
	n(x: AldorDomainRep): DomainName == name x;
	ag(d: AldorDomainRep, pc: DomainPtr, n: Hash, t: Hash, 
	   b: Box, flg: Bit): Box == {
		   newbox: Box := get(d, pc pretend Domain, n, t, b, flg);
		   newbox;
        }
	h(x: AldorDomainRep): Hash == WRAPP("Hash", hash x);
	i(dom: AldorDomainRep, child: DomainPtr): Domain == {
	     fn(d: AldorDomainRep): ((AldorDomainRep, Hash) -> ()) == {
	     	   expandInherited(d, dom, child pretend Domain);
	     }
	     dr := new(fn)$AldorDomainRep;
	     domainMakeDispatch dr;
	}
	new(n pretend DomNamer,
	    ag pretend DomGetter, -- get pretend DomGetter,
	    h pretend DomHasher, -- hash@(AldorDomainRep->Hash) pretend DomHasher,
	    i pretend DomInheritTo --inheritTo@((AldorDomainRep, Domain)->Domain) pretend DomInheritTo
				);

}

extendDispatchVector () : DV == {
	--free edv: DV;
	--not edv => 
	n(e: AldorDomainRep): DomainName == nameExtend(e);
	extendg(d: AldorDomainRep, pc: DomainPtr, n: Hash, t: Hash, b: Box, flg: Bit): Box ==
	     WRAPP("Getextend", getExtend(d, pc pretend Domain, n, t, b, flg));
	h(x: AldorDomainRep): Hash == WRAPP("hash extend", hashExtend x);
	--i(x: AldorDomainRep, d: DomainPtr): Domain == inheritToExtend(x,d pretend Domain);
	i(dom: AldorDomainRep, child: DomainPtr): Domain == {
	     efn(d: AldorDomainRep): ((AldorDomainRep, Hash) -> ()) == {
	     	   expandExtend(d, dom, child pretend Domain);
	     }
	     dr := new(efn)$AldorDomainRep;
	     extendMakeDispatch dr;
	}
	local edv: DV := new(n pretend DomNamer, 
		       extendg pretend DomGetter, -- getExtend pretend DomGetter,
		       h pretend DomHasher, -- hashExtend pretend DomHasher,
		       i pretend DomInheritTo --inheritToExtend pretend DomInheritTo
			 );
	edv;
}

dummyDispatchVector () : DV == {
        n(x: AldorDomainRep): DomainName == nameDummy x;
	dummyg(d: AldorDomainRep, pc: DomainPtr, n: Hash, t: Hash, b: Box, flg: Bit): Box ==
	     getDummy(d, pc pretend Domain, n, t, b, flg);
	h(x: AldorDomainRep): Hash == hashDummy x;
	i(x: AldorDomainRep, d: DomainPtr): Domain == inheritToDummy(x,d pretend Domain);
	local edv: DV := new(n pretend DomNamer, 
		       dummyg pretend DomGetter, -- getDummy pretend DomGetter,
		       h pretend DomHasher, -- hashDummy pretend DomHasher,
		       i pretend DomInheritTo --inheritToExtendDummy pretend DomInheritTo
			 );
	edv;
}

pointerDV() : DV == {
	--free pdv: DV;
	--not pdv => 
	--pdv := new(name@(%->DomainName) pretend DomNamer,
	--		      get pretend DomGetter,
	--		      domHash@(%->Hash) pretend DomHasher,
	--		      inheritTo@((%, Domain)->Domain)
	--				  pretend DomInheritTo
	--			);
	--pdv;
	import from Pointer;
	n(x: PointerDomain): DomainName == name x;
	ptrg(d: PointerDomain, pc: DomainPtr, n: Hash, t: Hash, b: Box, flg: Bit): Box ==
            get(d, pc pretend Domain, n, t, b, flg);
        h(x: PointerDomain): Hash == WRAPP("Hash pointer", hash x);
	bf(x: DomainRep): Domain == new(pointerDV(), x);
	tmpfn(d1: Domain, d2: Domain): Domain == {
		  fn(): Domain == {
		  	inheritTo(d1, d2);
	          }
		  x: Domain := new(pointerDV(), new(fn)$PointerDomain);
		  x
        }
        i(x: PointerDomain, d: DomainPtr): Domain == inheritTo(tmpfn, x,d pretend Domain);
	local edv: DV := new(n pretend DomNamer, 
		       ptrg pretend DomGetter, -- getExtend pretend DomGetter,
		       h pretend DomHasher, -- hashExtend pretend DomHasher,
		       i pretend DomInheritTo --inheritToExtend pretend DomInheritTo
			 );
	edv;
}

--
-- :: Category Dispatch vectors
--

-- Dispatch vector creation operations.

--!! These functions are here rather than in CatDispatchVector so
--!! we can initialize the functions used to construct the vector
--!! using global constants instead of using calls to domainGetExport!.

aldorCatDispatchVector(): CDV == {
	import from Pointer;
        local dv: CDV;
	n1(x: AldorCatRep): DomainName == name0 x;
	--b1(x: AldorDomainRep, d: Domain): CatPtr == {
	--      free dv: DV;
	--      build0(dv, x, d) pretend CatPtr;
	--}
	g1(x: AldorCatRep, d: Domain, h: Hash, h2: Hash, b: Box): Box == get0(x, d, h, h2, b);
	h1(x: AldorCatRep): Int == hash0 x;
	pc1(x: AldorCatRep): Int == parentCount0 x;
	pg1(x: AldorCatRep, i: Int): CatPtr == parentGet0(x, i) pretend CatPtr;
	b1(r: AldorCatRep, dom: Domain): CatPtr == build0(aldorCatDispatchVector, r, dom) pretend CatPtr;
	dv := new(n1  pretend CatNamer,
			    b1 pretend CatBuilder,
			    g1   pretend CatGetter,
			    h1  pretend CatHasher,
			    pc1 pretend CatParentCounter,
			    pg1    pretend CatParentGetter);
	dv;
}

dummyCatDispatchVector (): CDV == {
	n0(x: AldorCatRep): DomainName == nameDummy x;
	b0(x: AldorCatRep, d: Domain): CatPtr == buildDummy(x, d) pretend CatPtr;
	g0(x: AldorCatRep, d: Domain, h: Hash, h2: Hash, b: Box): Box == getDummy(x, d, h, h2, b);
	h0(x: AldorDomainRep): Int == hashDummy x;
	pc0(x: AldorCatRep): Int == parentCountDummy x;
	pg0(x: AldorCatRep, i: Int): CatPtr == parentGetDummy(x, i) pretend CatPtr;
	dv := new(n0  pretend CatNamer,
			    b0  pretend CatBuilder,
			    g0  pretend CatGetter,
			    h0  pretend CatHasher,
			    pc0 pretend CatParentCounter,
			    pg0 pretend CatParentGetter);
	dv;
		
}

--
-- :: Names
--


export {
	namePartConcat:   (Boolean, Tuple DomainName) -> DomainName;
	namePartFrOther:  Pointer	   -> DomainName;
	namePartFrString: String	   -> DomainName;
} to Foreign Builtin;

namePartConcat(x: Boolean, T: Tuple DomainName): DomainName == {
	import from List DomainName;
	combine(x, [T]);
}

namePartFrString(s: String): DomainName == new s;

namePartFrOther(p: Pointer): DomainName == other p;


--
-- :: String hash functions
--

export {
	stringHash: String -> SingleInteger;
} from Foreign Builtin;

import {
	fiStrHash: String -> SingleInteger
} from Foreign;

stringHash(s: String): SingleInteger == fiStrHash s;


--
-- :: Print assertion message
--

export {
	rtAssertMessage: (String, SingleInteger, String) -> ();
} from Foreign Builtin;

rtAssertMessage(file: String, line: SingleInteger, msg: String): () == {
	PRINT() << "Assertion failed at " << file << ":" << line << ": " << msg << NL();
}
