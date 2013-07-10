-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase linear
#pile

--% Linear1: Basic piling

-- The rain in spain falls mainly
#lisp (BREAK)
f(x) ==
	if i > 0 then
    -- ignore this
		x := 1
		y := 2
	else
		z := 3

List(S): with
	cons: (S, $) -> $
	first: $ -> S
   == add
	cons(a,b) == [a,:b]
	first l	==l.0

aaaaaaaa
aaaaaaaa
aaaaaaaa
    bbbbbbbbbb
	cccccccccccccc
	cccccccccccccc
	cccccccccccccc
    dddddddddd
  eeeeeeee
  eeeeeeee

if a then 
	if b then c else d
else
	if b then c else d
