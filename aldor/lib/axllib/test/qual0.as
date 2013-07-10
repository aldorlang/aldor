-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib
#pile

#include "axllib"

macro I == Integer$AxlLib

f(n: I): I ==
	n =$I 0$I => 1$I
	n *$I f(n -$I 1$I)

g(): () ==
	import from AxlLib
	import from Integer
	print << f 3 <<newline

g()
