#include "runtimelib.as"

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

		== per[axiomxlCatDispatchTag, namer, nil(), getter, hasher,
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
