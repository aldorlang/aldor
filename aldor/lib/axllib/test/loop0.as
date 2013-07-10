-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l
--> testgen c
--> testrun -l axllib
#pile

#include "axllib.as"
import from Segment Integer

pow1(i: Integer, j: Integer): Integer ==
	prod: Integer := 1
	while j > 0 repeat
		prod := i * prod
		j := j - 1
	prod

pow2(i: Integer, j: Integer): Integer ==
	prod: Integer := 1
	while j > 0 while (prod := i * prod; true) repeat (j := j-1)
	prod

pow3(i: Integer, j: Integer): Integer ==
	prod: Integer := 1
	for k in 1..j repeat
		prod := i * prod
	prod

pow4(i: Integer, j: Integer): Integer ==
	prod: Integer := 1
	for h in 2..j+1 for k in 1..j | h > k while k <= h repeat
		prod := i * prod
	prod

import from Integer

print<<pow1(3,4)<<newline
print<<pow2(3,4)<<newline
print<<pow3(3,4)<<newline
print<<pow4(3,4)<<newline
