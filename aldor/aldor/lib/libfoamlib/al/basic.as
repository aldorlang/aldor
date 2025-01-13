-----------------------------------------------------------------------------
----
---- basic.as: Basic Types
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

--- !!!!This is temporary and should be simplified by removing 
---     string operations, infile, etc.

#assert AssertConfig
#assert LibraryMachine
#assert AllMacros

#include "foamlib"

--)exn Exception
--)exn ErrorException
--)dom Boolean
--)...

+++ The Boolean data type supports logical operations.
+++ Both arguments of the binary operations are always evaluated.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: logical operation

Boolean: with {
	coerce:		BBool -> %;
	coerce:		% -> BBool;
}
== add {
	Rep ==> BBool;

	coerce(b: %): BBool   == rep b;
	coerce(b: BBool): %   == per b;
}


+++ Byte implements single byte integers. Typically 8 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: byte, single byte integer

Byte: with {
	coerce:		% -> BByte;
	coerce:		BByte -> %;
}
== add {
	Rep ==> BByte;

	coerce(b: %): BByte	== rep b;
	coerce(b: BByte): %	== per b;
}


+++ HalfInteger implements half-precision integers.  Typically 16 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: half-precision integer

HalfInteger: with {
	coerce:		% -> BHInt;
	coerce:		BHInt -> %;
}
== add {
	Rep ==> BHInt;

	coerce(h: %): BHInt	== rep h;
	coerce(h: BHInt): %	== per h;
}


+++ SingleInteger implements single-precision integers.  Typically 32 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: single-precision integer

SingleInteger: with {
	coerce:		Boolean     -> %;
	coerce:		Byte	    -> %;
	coerce:		HalfInteger -> %;

	coerce:		% -> BSInt;
	coerce:		BSInt -> %;
}
== add {
	Rep ==> BSInt;

	coerce(b: Boolean): %	  == per(if b then 1 else 0);
	coerce(b: Byte): %	  == per convert(b::BByte);
	coerce(h: HalfInteger): % == per convert(h::BHInt);
	coerce(i: BSInt): %	  == per i;
	coerce(i: %): BSInt	  == rep i;
}


+++ Integer implements infinite precision integers.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: infinite precision integer

Integer: with {
        integer:	Literal -> %;

	coerce:		SingleInteger -> %;

	coerce:		% -> BBInt;
	coerce:		BBInt -> %;
}
== add {
	Rep ==> BBInt;

	integer(l: Literal): % == per convert (l pretend BArr);

	coerce (i: SingleInteger): % == per convert(i::BSInt);

	coerce(i: %): BBInt == rep i;
	coerce(i: BBInt): % == per i;
}


+++ Characters for natural language text.
+++
+++ In the portable byte code files, characters are represented in ASCII.
+++ In a running program, characters are represented according to the
+++ machine's native character set, e.g. ASCII or EBCDIC.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: character, text, ASCII, EBCDIC

Character: with {
	ord:		% -> SingleInteger;
	char:		String -> %;
	char:		SingleInteger -> %;

	coerce:		% -> BChar;
	coerce:		BChar -> %;
}
== add {
	import from SingleInteger;

	Rep ==> BChar;

	ord(c: %): SingleInteger  == ord(rep c)::SingleInteger;
	char(i: SingleInteger):%  == per char(i::BSInt);
	char(s: String): %        == s.(1::SingleInteger);

	coerce(c: BChar): % == per c;
	coerce(c: %): BChar == rep c;
}


+++ Pointer is the type of pointers to opaque objects.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: pointer

Pointer: with == add;


+++ The `error' function displays a string and exits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: error, message, halt

error(s: String): Exit == {
	import {
		stdoutFile:	() -> Pointer;
		fputc:     (SingleInteger, Pointer) -> SingleInteger;
		fputs:     (String, Pointer) -> SingleInteger;
	
	} from Foreign;

	import from SingleInteger;

	fputs(s, stdoutFile());
	fputc(ord newline::SingleInteger, stdoutFile());
	halt 0;
}


+++ `TextWriter' is a type of writable text streams.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: file, stream, text, output, write, print

TextWriter: with {
	SI ==> SingleInteger;

	writer: (Character->(), (String, SI, SI) -> SI) -> %;
		++ `writer' forms a new TextWriter from functions.
		++
		++ The first function writes a character.
		++ The second function takes parameters `(s,i1,iL)'
		++ and writes the substring `s(i1..min(iL-1, #s))'.
		++ `iL=0' writes everthing up to the end of `s'.

	write!:	(%, Character) -> ();
		++ `write!(w, c)' puts the Character `c' on the TextWriter `w'.

	write!:	(%, String) -> ();
		++ `write!(w, s)' puts the String `s' on the TextWriter `w'.

	write!:	(%, String, SI, SI) -> SI;
		++ `write!(w,s,i1,iL)' puts `s(i1..min(iL-1, #s))' on the 
		++ TextWriter `w' and returns the number of characters written.
		++ `iL=0' writes everthing up to the end of `s'.

	#:	(%) -> SingleInteger;
}
== add {
	SI     ==> SingleInteger;
	OPRets ==> (Character->(), (String, SI,SI)->SI, ()->SI);
	Rep    ==> () -> OPRets;

	import from SI;

	writer(pc: Character->(), ps: (String, SI, SI)->SI): % == {
		n: BSInt := 0;
		fc(c: Character): () == {
			pc c;
			free n := n + 1
		}
		fs(s: String, start: SI, limit: SI): SI == {
			n1 := ps(s, start, limit)::BSInt;
			free n := n + n1;
			n1::SI
		}
		fn(): SI == n::SI;
		per ((): OPRets +-> (fc, fs, fn))
	}

	write!(w: %, c: Character): () ==
		{ (fc, fs, fn) := rep(w)(); fc c }
	write!(w: %, s: String): () ==
		{ (fc, fs, fn) := rep(w)(); fs(s, 1::SI, 0::SI) }
	write!(w: %, s: String, start:SI, limit:SI): SI ==
		{ (fc, fs, fn) := rep(w)(); fs(s, start, limit) }
	#(w: %): SI == 
		{ (fc, fs, fn) := rep(w)(); fn() }
}

+++`FormattedOutput' provides functions which format their arguments
+++ according to a control string.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: format, output

FormattedOutput: with {
}
== add {
}

+++ String is the type of character strings for natural language text.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: string, text

String: with {
	string:	Literal -> %;
		++`"hello"' is a string-style literal constant.

        data: % -> BArr;
		++`data s' returns a structure with the raw data for `s'.

        #: % -> SingleInteger;
                ++`#s' returns the number of characters in the string `s'.

        end?: (%, SingleInteger) -> Boolean;
                ++`end?(s,i)' is true when `i = #s+1'. This function may be
		++ called with values of `i' in the range `1..#s+1'.

	end!: (%, SingleInteger) -> ();
		++`end!(s,i)' causes the contents of `s' to end at position `i'.

	empty: SingleInteger -> %;
		++`empty n' allocates a new string with room for upto
		++ `n' characters.

        dispose!: % -> ();
                ++`dispose! s' indicates that `s' will no longer be used.

        apply: (%, SingleInteger) -> Character;
                ++`s.i' returns the `i'-th character from the string `s'.
                ++ The index `i' may take any value from `1' to `#s'.

        set!: (%, SingleInteger, Character) -> Character;
		++`s.i := c' updates the `i'-th character position in `s' to
		++ contain the value `c'.

	<<: (TextWriter, %) -> TextWriter;
		++`w << s' emits the string `s' on the writer `w'.
}
== add {
	import from Character;

        EOS():Character == char(0)::Character;

	Rep ==> BArr;

        import {
                ArrNew: (BChar, BSInt) -> BArr;
                ArrElt: (BArr,  BSInt) -> BChar;
                ArrSet: (BArr,  BSInt, BChar) -> BChar;
        } from Builtin;

	string(l: Literal): % == l pretend %;

        apply(s: %, i: SingleInteger): Character ==
                ArrElt(rep s, i::BSInt-1)::Character;

        set!(s: %, i: SingleInteger, c: Character): Character ==
                ArrSet(rep s, i::BSInt-1, c::BChar)::Character;

        end?(s: %, i: SingleInteger): Boolean ==
		(ArrElt(rep s, i::BSInt-1) = EOS()::BChar)::Boolean;

        end!(s: %, i: SingleInteger): () ==
		ArrSet(rep s, i::BSInt-1, EOS()::BChar);

        empty(size: SingleInteger): % == {
                s := per ArrNew(EOS()::BChar, size::BSInt+1);
                ArrSet(rep s, 0,           EOS()::BChar);
                ArrSet(rep s, size::BSInt, EOS()::BChar);
                s
        }

        dispose!(s: %): () ==
		dispose! rep s;

        #(s: %): SingleInteger == {
		i: BSInt := 1;
		while not end?(s, i::SingleInteger) repeat i := i + 1;
		(i - 1)::SingleInteger
	}

        data(s: %): BArr == rep s;

	(w: TextWriter) << (s: %): TextWriter == { write!(w, s); w }
}


+++
+++ `RoundingMode'.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1994
+++ Keywords: floating point, real, approximate, arithmetic

RoundingMode: with {
	zero: 		() -> %;
	nearest: 	() -> %;
	up: 		() -> %;
	down:		() -> %;
	any: 		() -> %;

	coerce: 	%  		-> BSInt;
	=:		(%,%) 		-> Boolean;
	~=:		(%,%) 		-> Boolean;
	<<:		(TextWriter,%) 	-> TextWriter;

} == add {
	Rep==>BSInt;
	import from Machine, String;

	zero():% 	== per Bzero();
	nearest():% 	== per Bnearest();
	up():% 		== per Bup();
	down():% 	== per Bdown();
	any():% 	== per Bany();

	coerce(x:%):BSInt == rep x;

	(a:%) = (b:%) : Boolean == ((rep a) = (rep b)) :: Boolean;
	(a:%) ~= (b:%) : Boolean == ((rep a) ~= (rep b)):: Boolean;

	-- sample: % == any();

	(t:TextWriter) << (r :%) : TextWriter == { 
		t << (
		r = zero() => "zero";
		r = nearest() => "nearest";
		r = down() => "down";
		r = up() => "up";
		r = any() => "any";
		error "Unknown mode"
		)
	}

}

