-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib"

import from SingleInteger
import from Integer

a : SingleInteger := 1

foo ():Integer ==
	free a,b
	a : Integer := 1
	b : Integer := a
