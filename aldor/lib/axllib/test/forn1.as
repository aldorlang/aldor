-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testgen c

#include "axllib.as"
#pile

import
	puts: String -> SingleInteger
from Foreign C

puts(n: Integer): Integer == n

import from String

puts "Hello world!"

