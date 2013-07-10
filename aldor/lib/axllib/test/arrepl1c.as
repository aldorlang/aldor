-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib"
#library ReplLib "repl"
import from ReplLib;

Int ==> SingleInteger;

D: with {
	m: Int;
	n: Int;
	export from B;
}
== add {
	import from B;
	m: Int == a quo b;
	n: Int == a rem b;
}
