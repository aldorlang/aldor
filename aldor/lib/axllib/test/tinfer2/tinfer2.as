-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#include "axllib.as"
#pile

------ The coerce should not be ambiguous
n:Integer :=5
import from DoubleFloat
n2:=n+2
x1:=n2::DoubleFloat

------ Selecting function fields from records must drop the tag.
r: Record(inc: Integer -> Integer) := [ (n:Integer):Integer +-> n+1 ]
r.inc 4

------ Ifs in value context must unify and disambiguate their branches
macro I == Integer
macro R == Record(a:I,b:I)
1:R == [1$I,0$I]
-(r: R): R == r
(r: R) mod (s: R): R == r

f(x:R):I == if x.a < 0 then -1 else 1
g(x:R):I ==
	i := if x.a < 0 then
		1 mod 1		-- I or R
	else
		gcd(1, 1)	-- I or DF
	i
