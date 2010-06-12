
#include "foamlib"
#include "runtimelib"

--!! This unmitigated hack is to get around inliner limitation to functions.
--!!\begin{hack}
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


+++ Domain is the top-level domain representaion, designed to operate in
+++ an environment of mixed runtime systems.  The domain consists of
+++ a pointer to the domain's native representation, and a vector of
+++ functions for accessing it.  Currently only "get" and "hash" functions
+++ are required.
Domain: Conditional with {
	new: (DispatchVector, DomainRep) -> %;
#if ExtendReplace
	copy:			% -> %;
		++ copy(dom) creates a new domain object using
		++ the dispatch vector and DomainRep from dom.

	ncopy:			(%, %) -> ();
		++ ncopy(dst, src) destructively copies
		++ the dispatch vector and DomainRep from src to dst.
#endif

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

	fill!:		(%, %) -> ();
	reFill!:	(%, DispatchVector, DomainRep) -> ();
	domainRep:	% -> DomainRep;
}
== add {
	Rep ==> Record (dispatcher:	DispatchVector, 
			domainRep:	DomainRep);

	import from Rep;

	domainRep  (td: %): DomainRep		== rep(td).domainRep;
	dispatcher (td: %): DispatchVector	== rep(td).dispatcher;


	new (dv: DispatchVector, d: DomainRep) : % ==
		per [dv, d];


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
		val := get(domainRep td, td pretend DomainPtr, 
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
--		import from StringTable;
--		PRINT() << "Looking in ";
--		printDomain(PRINT(), getName td)
--                 	<< " for "       << find name
--                      << " with code " << type << NL();
	}		

	getExportInner! (td: %, pcent: %, name: Hash, type: Hash, box: Box, skip: Bit)
			: Box ==
		(getter dispatcher td)(domainRep td, pcent pretend DomainPtr, 
				   		name, type, box, skip);

	getHash! (td: %) : Hash ==
		(hasher dispatcher td)(domainRep td);

	testExport! (td: %, name: Hash, type: Hash) : Bit ==
		test getExport0!(td, name, type);

	getName(td: %): DomainName == {
		 f := (namer dispatcher td);
		 f(domainRep td);
        }

	inheritTo(td: %, dom2: Domain): Domain == {
		f := (inheriter dispatcher td);
		f(domainRep td, dom2 pretend DomainPtr) pretend Domain;
	}

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

makeDomain(dv: DispatchVector, r: DomainRep): Domain ==
        new(dv, r);
