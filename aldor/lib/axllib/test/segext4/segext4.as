-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib	"lang.ao"
#library CliqueLib	"basecliq.ao"
#library SegmentLib	"seg.ao"
#library ExtendLib	"segext3.ao"

import from LangLib, CliqueLib, SegmentLib, ExtendLib;

I ==> Integer;

f(n: I): I == {
	import from Segment I;

	sum: I := 0;
	for i in 1..n repeat sum := sum + i;
	sum;
}
