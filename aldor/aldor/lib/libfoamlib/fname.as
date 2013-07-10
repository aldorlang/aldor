-----------------------------------------------------------------------------
----
---- fname.as: Portable file name manipulation
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

S ==> String;

FileName: BasicType with {
	filename:  S -> %;
	filename:  (dir: S, name:S, type:S) -> %;

	dir:	   % -> S;
	name:	   % -> S;
	type:	   % -> S;

	unparse:   % -> S;

}
== add {
	Rep ==> Record(dir: S, name: S, type: S, native: S);

	import from Rep;
	import from String;
	import from Pointer;
	import from SingleInteger;
	import from PrimitiveArray S;
	import from OperatingSystemInterface;

	sample: % == filename("a", "b", "c"); 

	filename(s: S): %   == {
		fnparts: PrimitiveArray String := new(fnameNParts, "");
		flen := fnameParseSize(s, nil pretend String);
		fnameParse(fnparts, empty flen, s, nil pretend String);
		fn := per [fnparts.1, fnparts.2, fnparts.3, copy s];
		dispose! fnparts;
		fn;
	}
	filename(dir: S, name: S, type: S): % == {
		fnparts: PrimitiveArray String := new(fnameNParts, "");
		fnparts.1 := dir;
		fnparts.2 := name;
		fnparts.3 := type;
		flen := fnameUnparseSize(fnparts, false);
		s    := fnameUnparse(empty flen , fnparts, false);
		fn   := per [dir, name, type, s];
		dispose! fnparts;
		fn
	}

	dir    (fn: %): S == rep(fn).dir;
	name   (fn: %): S == rep(fn).name;
	type   (fn: %): S == rep(fn).type;
	unparse(fn: %): S == rep(fn).native;


	(fn1: %)  = (fn2: %): Boolean == unparse fn1  = unparse fn2;
	(fn1: %) ~= (fn2: %): Boolean == unparse fn1 ~= unparse fn2;

	(p: TextWriter) << (fn: %): TextWriter ==
		p << "_"" << unparse fn << "_"";	--!! Should escape "'s


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}
