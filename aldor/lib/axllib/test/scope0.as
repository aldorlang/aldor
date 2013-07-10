-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

z: Boolean == false
z: Integer == 0

left(i: Integer, b: Boolean): Integer == i
left(b1:Boolean, b2:Boolean): Boolean == b1
left(b: Boolean, i: Integer): Boolean == b

f(i: Integer, b: Boolean): Integer ==
	b := left(z, b)           	-- lib-b+zi or lbb-b+zb: choose b
	i := left(z, b)		     	-- same but choose i
	b := left(left(z, b), b)  	-- lib-b+zi or lbb-b+zb: choose b
	i := left(left(z, b), b)  	-- same but choose i
