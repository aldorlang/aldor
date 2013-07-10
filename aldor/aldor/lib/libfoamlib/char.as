-----------------------------------------------------------------------------
----
---- char.as: Extend the Character type with basic operations.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

+++ Characters for natural language text.
+++
+++ In the portable byte code files, characters are represented in ASCII.
+++ In a running program, characters are represented according to the
+++ machine's native character set, e.g. ASCII or EBCDIC.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: character, text, ASCII, EBCDIC

extend Character: OrderedFinite with {
	space:	 %;
	newline: %;
	tab: %;
	digit?:	 % -> Boolean;
	letter?: % -> Boolean;
	lower:	 % -> %;
	upper:	 % -> %;
}
== add {
	-- Rep ==> BChar;
	Rep == BChar;
	import from Machine;
	import from SingleInteger;

	sample:  %                == per space;
	space:	 %		  == per space;
	newline: %		  == per newline;
	tab: 	 %		  == per tab;
	min:	 %		  == per min;
	max:	 %		  == per max;
	#:	 Integer	  ==
		(ord max - ord min + 1)@BSInt::SingleInteger::Integer;

	digit? (c: %): Boolean    == digit? (rep c)::Boolean;
	letter?(c: %): Boolean    == letter?(rep c)::Boolean;

	(a: %) =  (b: %): Boolean == (rep a =  rep b)::Boolean;
	(a: %) ~= (b: %): Boolean == (rep a ~= rep b)::Boolean;
	(a: %) <  (b: %): Boolean == (rep a <  rep b)::Boolean;
	(a: %) >  (b: %): Boolean == (rep b <  rep a)::Boolean;
	(a: %) <= (b: %): Boolean == (rep a <= rep b)::Boolean;
	(a: %) >= (b: %): Boolean == (rep b <= rep a)::Boolean;

	lower(c: %): % == per lower rep c;
	upper(c: %): % == per upper rep c;
	(w: TextWriter) << (c: %): TextWriter == 
		{ write!(w, c); w }


	-- Originally these were defaults from BasicType etc
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
	max(x: %, y: %): % == if x > y then x else y;
	min(x: %, y: %): % == if x < y then x else y;
}
