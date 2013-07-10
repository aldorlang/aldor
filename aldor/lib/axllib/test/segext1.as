-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#library LangLib	"lang.ao"
#library SegmentLib	"segext0.ao"

import from LangLib, SegmentLib;
import from Boolean;

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
