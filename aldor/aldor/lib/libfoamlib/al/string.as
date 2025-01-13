-----------------------------------------------------------------------------
----
---- string.as:
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#assert NImportString
#include "foamlib"

+++ String is the type of character strings for natural language text.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-97
+++ Keywords: string, text

extend String: BasicType with {

        new: (SingleInteger, fill: Character == space) -> %;
                ++ `new(n, c)' creates a new string filled with `c' characters.

	string: Array Character -> %;
		++ `string ac' creates a string out of an array of characters.

	copy: % -> %;
		++ `copy(s)' creates a new string containing a copy of `s'.

	concat: Tuple % -> %;
		++ `concat(s1,s2,...)' creates a new string containing the
		++ characters of `s1' followed by the characters of `s2', etc.

	apply: (%, OpenSegment   SingleInteger) -> %;
		++ `s.(i..)' allocates a new string containing the substring
		++ with characters `s.i' to `s.(#s)'.

	apply: (%, ClosedSegment SingleInteger) -> %;
		++ `s.(i..j)' allocates a new string containing the substring
		++ with characters `s.i' to `s.j'.

	generator: % -> Generator Character;

	map:  (Character -> Character, %) -> %;
		++ `map!(f, s)' forms a new string from `s' with each
		++ character `c' of the old string replaced by `f c' in the new.

	map!: (Character -> Character, %) -> %;
		++ `map!(f, s)' modifies the string `s' so that each
		++ character `c' is replaced by `f c'.

	fill!: (%, Segment SingleInteger, Character) -> %;

	replace!: (%, SingleInteger, %) -> SingleInteger;
		++ `replace!(s1, i0, s2)' replaces characters in `s1'
		++ begining at position `i0' with characters from `s2'.
		++ Returns the next available position in `s1'.

	substring: (%, SingleInteger, SingleInteger) -> %;
		++ `substring(s, start, len)' returns the substring of `s'
		++ of length `len' beginning at position `start'.

        rightTrim: (%, Character) -> %;
                ++ `rightTrim(s,c)' removes trailing occurrences
		++ of `c' from `s'.

        leftTrim: (%, Character) -> %;
                ++ `leftTrim(s,c)' removes leading  occurrences
		++ of `c' from `s'.

        +: (%, %) -> %;
}
== add {
	import from Machine;
	import from Boolean, Character, SingleInteger, Segment SingleInteger;

	new(size: SingleInteger, fill: Character == space): % == {
		s := empty size;
		for i in 1..size repeat s.i := fill;
		s
	}
	string(a: Array Character): % == {
		l := #a;
		r: % := empty l;
		for i in 1..l repeat r.i := a.i;
		r
	}
	copy(s: %): % == {
		len := #s;
		r   := empty len;
		for i in 1..len repeat r.i := s.i;
		r
	}
	(s1: %) = (s2: %): Boolean == {
		for i in 1.. repeat {
			end?(s1, i)  => return end?(s2, i);
			s1.i ~= s2.i => return false;
		}
		true
	}
	replace!(s1: %, pos: SingleInteger, sub: %): SingleInteger == {
		for i in 1.. for ii in pos.. repeat {
			end?(sub, i) or end?(s1, ii) => return ii;
			s1.ii := sub.i;
		}
		never
	}
	substring(s1: %, pos: SingleInteger, len: SingleInteger): % == {
		s2 := empty len;
		for i in 1..len for j in pos.. repeat
			s2.i := s1.j;
		s2
	}
	rightTrim(s: %, c: Character): % == {
		len := #s;
		for i in len..1 by -1 repeat
		     s.i ~= c => return substring(s, 1, i);
		"";
	}
	leftTrim(s: %, c: Character): % == {
		len := #s;
		for i in 1..len repeat
		     s.i ~= c => return substring(s, i, len-i+1);
		"";
	}
	apply(s: %, seg: OpenSegment SingleInteger): % == {
		pos := low(seg::Segment SingleInteger);
		len := #s - pos + 1;
		substring(s, pos, len);
	}
	apply(s: %, seg: ClosedSegment SingleInteger): % == {
		pos := low (seg::Segment SingleInteger);
		len := high(seg::Segment SingleInteger) - pos + 1;
		substring(s, pos, len);
	}
	fill!(s: %, seg: Segment SingleInteger, c: Character): % == {
		len := #s;
		for i in seg while i <= len repeat s.i := c;
		s
	}

        (+)(a: %, b: %): % == concat(a, b);

	concat(t: Tuple %): % == {
                n : SingleInteger := 0;
                for i in 1..length t repeat n := n + #element(t, i);
                r := new n;
                n := 1;
                for i in 1..length t repeat n := replace!(r, n, element(t, i));
                r
	}
	map!(f: Character -> Character, s: %): % == {
		for i in 1.. repeat {
			end?(s, i) => return s;
			s.i := f s.i;
		}
		s
	}
	map(f: Character -> Character, s: %): % == {
		s2 := empty(l := #s);
		for i in 1..l repeat s2.i := f s.i;
		s2
	}
	generator(s: %): Generator Character == generate {
		for i in 1.. while not end?(s,i) repeat yield s.i;
	}
	sample: % == empty 0;

        #(s: %): SingleInteger == {
		i: BSInt := 1;
		while not end?(s, i::SingleInteger) repeat i := i + 1;
		(i - 1)::SingleInteger
	}

	hash(s: %): SingleInteger == {
		import {
			fiStrHash: String -> SingleInteger
		} from Foreign;
		fiStrHash(s);
	}


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}
