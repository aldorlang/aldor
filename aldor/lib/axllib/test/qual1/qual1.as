-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

#library ILib "integer.ao"
#library SILib "sinteger.ao"

macro {
	SI == SingleInteger;
	I  == Integer;
}

bi := 0$I
si := 0$SI
b0 := 0$I$ILib

f(): () ==
	import from I$ILib
	import from SI$SILib

	print<<bi<<newline
	print<<si<<newline
	print<<b0<<newline

#if TestErrorsToo
noSuchExport$I + 0$NoSuchType
#endif
