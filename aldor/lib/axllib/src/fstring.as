
-----------------------------------------------------------------------------
----
---- fstring.as: strings of fixed length
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"


SI ==> SingleInteger;


+++ FixedString(n) is the type of character strings of fixed length n.
+++ We need this so we can communicate with Fortran functions that
+++ take or return such values. For simplicity we provide the simplest
+++ interface and let the user coerce to String for anything fancy.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-99
+++ Keywords: string, text, fixed, Fortran

FixedString(n:SI): Join(BasicType, FortranFString) with {
	coerce : % -> String;
		++ Convert to an Aldor String value. We take extra
		++ care incase someone has been fiddling with our
		++ representation without our knowledge.


	coerce : String -> %;
		++ Convert from a String value, truncating the
		++ result or padding it with spaces if necessary.

	# : % -> SI;
		++ The value of `n'. We don't need any parameters
		++ for this function really but it means that the
		++ syntax is consistent with String.
}
== add
{
	Rep == String;
	import from Rep;

	#():BSInt == n::BSInt;
	#(s:%):SI == n;
	sample:% == per sample;

	(a:%) = (b:%):Boolean == (rep a) = (rep b);
	(t:TextWriter) << (s:%):TextWriter == t << (rep s);

	coerce(s:%):String ==
	{
		-- We cannot guarantee that our internal
		-- representation is still the length that
		-- we expect it to be (c.f. pretend).
		import from SI;
		(#s < n) => concat(rep s, new(n - #s));
		substring(rep s, 1, n);
	}

	coerce(s:String):% ==
	{
        	import from SI;
		(#s < n) => per concat(s, new(n - #s));
		per substring(s, 1, n);
	}


	import from Machine, Character;
	import
	{
		ArrNew: (BChar, BSInt) -> BArr;
		ArrElt: (BArr, BSInt) -> BChar;
		ArrSet: (BArr, BSInt, BChar) -> BChar;
	} from Builtin;


	PackedArrayNew(nelts:BSInt):BArr ==
		ArrNew(space, nelts*(n::BSInt));


	PackedArrayGet(a:BArr, i:BSInt):% ==
	{
		local off:BSInt := i*(n::BSInt);
		local chr:BChar;
		local result:String := new(n);

		for elt in 1..n repeat
		{
			chr := ArrElt(a, off + (elt::BSInt) - 1);
			result.elt := chr::Character;
		}

		per result;
	}


	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
	{
		local off:BSInt := i*(n::BSInt);
		local chr:BChar;
		local source:String := rep e;

		for elt in 1..n repeat
		{
			chr := (source.elt)::BChar;
			ArrSet(a, off + (elt::BSInt) - 1, chr);
		}

		e;
	}

	PackedRecordGet(p:BPtr):% ==
	{
		local chr:BChar;
		local result:String := new(n);

		for elt in 1..n repeat
		{
			chr := ArrElt(p pretend BArr, (elt::BSInt) - 1);
			result.elt := chr::Character;
		}

		per result;
	}


	PackedRecordSet(p:BPtr, v:%):% ==
	{
		local chr:BChar;
		local source:String := rep v;

		for elt in 1..n repeat
		{
			chr := (source.elt)::BChar;
			ArrSet(p pretend BArr, (elt::BSInt) - 1, chr);
		}

		v;
	}

	PackedRepSize():BSInt == (n::BSInt);
}
