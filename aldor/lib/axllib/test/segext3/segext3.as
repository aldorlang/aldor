-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib	"lang.ao"
#library CliqueLib	"basecliq.ao"
#library SegmentLib	"seg.ao"

import from LangLib, CliqueLib, SegmentLib;
Boolean:with == add;
import from Boolean;

S ==> _#1;

extend Segment (S: Type): with {
	if S has OrderedRing then {
		generator:	% -> Generator S;
	}
}
== add {
	if S has OrderedRing then {
		import from S;
		generator(x: %): Generator S == {
			l: S := lo x;
			h: S := hi x;
			inc: S := incr(x)::S;
			if l < h then
				generate while l <= h repeat {
					yield l;
					l := l + inc;
				}
			else
				generate while l >= h repeat {
					yield l;
					l := l + inc;
			}
		}
	}
}
