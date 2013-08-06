-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase linear
#pile

--% Linear2: Tricky piling: dangling elses, ++ comments, multi-line tuples
--
-- Note that the leading white space uses tabs.

f x ==
	if a
	  then
	    if b then c else d
	if A
	  then
	    if B then C
	  else D
	

	++ This comment
	++ goes with the following declaration
	f : T -> S ++ This
	           ++ was it


	++ This comment does not 
	++ go with the following definition
		++ Because it has a sub pile with neat stuff
		++ in it.
		x + y
	g : T -> S ++ But this
		   ++ gets attached.


	a :=  [ 1, 
		2,
		3,
		4 ]

	b := functionallyYours(
		foo,
		bar
	)


	if a 
    then wacky Stuff
  else abounds
