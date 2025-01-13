-----------------------------------------------------------------------------
----
---- segment.as:  A basic type for indicating ranges of discrete values.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

+++ Segment(S) provides ranges for indexing or iteration.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: segment, range

-- This is a temporary hack to see what gives...

define Steppable: Category == OrderedRing with { 
      stepsBetween: (%, %, %) -> SingleInteger;
      stepsBetween: (%,%) -> SingleInteger;
}

OpenSegment  (S: Steppable): BasicType with == Segment(S) add;
ClosedSegment(S: Steppable): BasicType with == Segment(S) add;

Segment(S: Steppable): BasicType with {
	..:	  (S)	 -> OpenSegment S;
	..:	  (S, S) -> ClosedSegment S;

	by:	  (%, S) -> %;
	by:	  (OpenSegment S,   S) -> %;
	by:	  (ClosedSegment S, S) -> %;

	open?:	  % -> Boolean;
	low:	  % -> S;
	high:	  % -> S;
	step:	  % -> S;

	generator: OpenSegment S   -> Generator S;
	generator: ClosedSegment S -> Generator S;
	generator: %               -> Generator S;

	coerce:   OpenSegment S   -> %;
	coerce:   ClosedSegment S -> %;
}
== add {
	Rep ==> Record(open?: Boolean, low: S, high: S, step: S);

	import from Rep;

	OSeg ==> OpenSegment S;
	CSeg ==> ClosedSegment S;

	sample: %	     == per [true, 0, 0, 1];

	(a:S) ..      :OSeg  == per [true,  a, 0, 1] pretend OSeg;
	(a:S) .. (b:S):CSeg  == per [false, a, b, 1] pretend CSeg;

	coerce(s:OSeg):%     == s pretend %;
	coerce(s:CSeg):%     == s pretend %;

	(s:OSeg) by (d:S):%  == (s::%) by d;
	(s:CSeg) by (d:S):%  == (s::%) by d;
	(s:%)    by (d:S):%  == per [open?(s), low s, high s, d*step s];

	open?(s: %): Boolean == rep(s).open?;
	low  (s: %): S	     == rep(s).low;
	high (s: %): S	     == rep(s).high;
	step (s: %): S	     == rep(s).step;

	generator(s: OSeg): Generator S == generate {
		a := low (s::%);
		repeat (yield a; a := a + 1);
	}

	generator(s: CSeg): Generator S == generate { -- to size s of {
		a := low (s::%);
		b := high(s::%);
		while a <= b repeat (yield a; a := a + 1);
	}

#if 0
	generator(s: %): Generator S == generate { -- to size s of {
		a := low  s;
		b := high s;
		d := step s;
		open? s =>		repeat (yield a; a := a + d);
		d >= 0	=> while a <= b repeat (yield a; a := a + d);
		d <  0	=> while a >= b repeat (yield a; a := a + d);
	}
#endif

	generator(s: %): Generator S == generate { -- to size s of {
		a := low  s;
		b := high s;
		d := step s;
		ubd := open? s;
		up := d >= 0;
		while ubd or (up and a <= b) or (not up and a >= b) repeat {
			yield a;
			a := a + d;
		}
	}

	(a: %) = (b: %): Boolean == {
		(aa, bb) := (rep a, rep b);
		aa.open? = bb.open?
			and aa.low  = bb.low  and  aa.high = bb.high
		   	and aa.step = bb.step;
	}

	(p: TextWriter) << (a: %): TextWriter == {
		import from String;
		aa := rep a;
		p << aa.low << "..";
		if not aa.open? then p << aa.high;
		if aa.step ~= 1 then p << " by " << aa.step;
		p;
	}

	size(s: %): SingleInteger    == stepsBetween(low s, high s, step s);
	size(s: CSeg): SingleInteger == stepsBetween(low (s::%),high (s::%),1);


	-- Originally these were defaults from BasicType
	(x: %) ~= (y: %): Boolean == not (x = y);
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}
