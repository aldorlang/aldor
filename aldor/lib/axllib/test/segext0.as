-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib	"lang.ao"
import from LangLib;

-- Needed for conditionals.
Boolean:with == add;
import from Boolean;

I ==> Integer;

OrderedRing: Category == with {
	0:	%;
	1:	%;
	+:	(%, %) -> %;
	<:	(%, %) -> Boolean;
	<=:	(%, %) -> Boolean;
	>=:	(%, %) -> Boolean;
	coerce:	I -> %;
}

export Integer: OrderedRing;

export Segment: (S: Type) -> with {
	export from S;
	SegmentCategory S;
	if S has OrderedRing then
		SegmentExpansionCategory(S pretend OrderedRing, S);
}

SegmentExpansionCategory (S: OrderedRing, T: Type) : Category == with {
	SegmentCategory S;
	map:	(S -> S, %) -> T;
}

SegmentCategory (S: Type) : Category == with {
	..:	(S, S) -> %;
	lo:	% -> S;
	hi:	% -> S;
	incr:	% -> I;
}
