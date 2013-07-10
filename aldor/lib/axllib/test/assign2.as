-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib.as"

y: Integer == 1
natascia():(Integer,Integer) == (10,7)
x2 := (y,y)

x2 := (1,1)

(x5,x6) := (1, natascia())

x3 := natascia()

printcross2(x1:Integer, x2:Integer):() ==
	print<<"("
	print<<x1
	print<<"--"
	print<<x2
	print<<")"

printcross2(x6)
printcross2(x3)
print<<x5

u : (Integer,Integer) == (1,2)

#if TestErrorsToo

local tascia: (Integer, Integer)

tascia := (1, 1)

x : (Integer, Integer) := (1,1)

#endif
