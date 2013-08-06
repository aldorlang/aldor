-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase linear
#pile

--% Linear3: Test piles inside enclosures.

a := [
	1,
	if a then
		2
		a
	else
		3
		b,		-- Syntax error: "else" encloses ","
	4,
	(
		if x then
			5
			c
		else
			6
			d
	),			-- Enclose the whole thing in parentheses.
	7,
	if u then
		8
		e
	else
		9
		f
	,			-- Or put the comma on a separate line.
	10
]
