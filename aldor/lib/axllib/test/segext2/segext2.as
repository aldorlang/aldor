-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib	"lang.ao"
#library SegmentLib	"segext0.ao"
#library ExtendLib	"segext1.ao"

import from LangLib, SegmentLib, ExtendLib;

I ==> Integer;

f(n: I): I == {
	import from Segment I;

	sum := 0;
	for i in 1..n repeat sum := sum + i;
	sum;
}
