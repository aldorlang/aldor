-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
----> testgen c -O -W floatrep
--> testgen c -O

-- Tests using foriegn functions as first-class values...

#include "axllib.as"
#pile

import { sin: BDFlo -> BDFlo;
	cos: BDFlo -> BDFlo;
	tan: BDFlo -> BDFlo } from Foreign(C);

import from DoubleFloat;
import from SingleInteger;

T1(): () == {
	t1(f: BDFlo->BDFlo): () == {
		for x: DoubleFloat in step(11)(0.0, 1.0) repeat {
			print <<x<<"->";
			print <<(f (x::BDFlo))::DoubleFloat;
			print <<newline
		}
	}
	t1(sin);	
	t1(cos);
	t1(tan);
}

T1();

import { zzz: String->(SingleInteger, SingleInteger) } from Foreign(C);

T2(): () == {
	print <<zzz pretend SingleInteger<<newline
}

