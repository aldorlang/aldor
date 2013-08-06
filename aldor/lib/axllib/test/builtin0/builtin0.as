-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testrun -O -l axllib
#pile

#include "axllib"

-- Test the execution of various builtin operations.

SI ==> SingleInteger
I  ==> Integer
SF ==> SingleFloat
DF ==> DoubleFloat

foo(): () ==
	import from Format
	local
		s: String
		si: SI
		bi: I
		sf: SF
		df: DF
		i:  SI

	si := 42
	bi := 42424242424242
	sf := 1.2
	df := 2.1
	s := new(100)
	i := format(si, s, 1)
	print<<"si = "<<si<<" s = "<<s<<" i = "<<i<<newline
	s := new(100)
	i := format(bi, s, 1)
	print<<"bi = "<<bi<<" s = "<<s<<" i = "<<i<<newline
	s := new(100)
	i := format(sf, s, 1)
	print<<"sf = "<<sf<<" s = "<<s<<" i = "<<i<<newline
	s := new(100)
	i := format(df, s, 1)
	print<<"df = "<<df<<" s = "<<s<<" i = "<<i<<newline

	s := "42"
	(si, i) := scan(s, 1)
	print<<"si = "<<si<<" s = "<<s<<" i = "<<i<<newline
	s := "420024"
	(bi, i) := scan(s, 1)
	print<<"bi = "<<bi<<" s = "<<s<<" i = "<<i<<newline
	s := "4.2"
	(sf, i) := scan(s, 1)
	print<<"sf = "<<sf<<" s = "<<s<<" i = "<<i<<newline
	s := "4.234"
	(df, i) := scan(s, 1)
	print<<"df = "<<df<<" s = "<<s<<" i = "<<i<<newline

foo()
