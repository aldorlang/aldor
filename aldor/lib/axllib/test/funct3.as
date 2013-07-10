-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib.as"

C0 : with
	+ : (% , %) -> Integer
  == add
	(x:%) + (y:%): Integer == 1

C1 : with  == add
	import from C0
	i:Integer == true + 1	
	b:Boolean == true + 1	



