-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

#include "axllib.as"

Pack(x:SingleInteger): with
    new: SingleInteger -> %
    <<: (TextWriter, %) -> TextWriter
 == add
     Rep ==> Record(value:SingleInteger)
     import from Rep
     new(y:SingleInteger):% == per [y]
     (p: TextWriter) << (v: %): TextWriter ==
	import from String
	p << "x = "<<x<<" val = "<<rep(v).value<<newline

import from SingleInteger
f(i:SingleInteger):Pack(i) == 
	n := new(i+1)
	print<<n<<newline
	n
f(2)
