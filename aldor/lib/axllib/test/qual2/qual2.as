-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

import from Integer;

D: with {
	d:	Integer -> %;
	<<:	(TextWriter, %) -> TextWriter;
	=:	(%, %) -> Boolean;
}
== add {
	Rep ==> Integer;
	d (x: Integer) : % == per x; 
	(p: TextWriter) << (x: %) : TextWriter == p << rep x;
	(x: %) = (y: %) : Boolean == rep x = rep y;
}

PrintCategory : Category == with {
	<<:	(TextWriter, %) -> TextWriter;
}

import PrintCategory from D;

x: D := d 3;

print<<x<<newline
