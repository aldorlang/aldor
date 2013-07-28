-----------------------------------------------------------------------------
----
---- format.as: Conversion of numbers to and from strings.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

+++ Text conversion: the second argument to each of the format
+++ functions MUST be writable. Note that since String literals
+++ are usually stored in read-only memory so they must NOT be
+++ used. A copy of a string literal will be okay.
Format: with {
	format:	(SingleFloat,   String, SingleInteger) -> SingleInteger;
	format:	(DoubleFloat,   String, SingleInteger) -> SingleInteger;
	format:	(SingleInteger, String, SingleInteger) -> SingleInteger;
	format:	(Integer,	String, SingleInteger) -> SingleInteger;

	scan:	(String, SingleInteger) -> (SingleFloat,   SingleInteger);
	scan:	(String, SingleInteger) -> (DoubleFloat,   SingleInteger);
	scan:	(String, SingleInteger) -> (SingleInteger, SingleInteger);
	scan:	(String, SingleInteger) -> (Integer,       SingleInteger);
}
== add {
	import from Machine;

	default sf: SingleFloat;
	default df: DoubleFloat;
	default si: SingleInteger;
	default bi: Integer;

	default s:  String;
	default i:  SingleInteger;
	default b:  BSInt;

	import { StoIsWritable: String -> SingleInteger} from Builtin;

	format(sf, s, i): SingleInteger == {
		assert(StoIsWritable(s) >= 0);
		format(sf::BSFlo, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	format(df, s, i): SingleInteger == {
		assert(StoIsWritable(s) >= 0);
		format(df::BDFlo, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	format(si, s, i): SingleInteger == {
		assert(StoIsWritable(s) >= 0);
		format(si::BSInt, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	format(bi, s, i): SingleInteger == {
		assert(StoIsWritable(s) >= 0);
		format(bi::BBInt, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	scan(s, i): (SingleFloat,   SingleInteger) == {
		(a: BSFlo, b) := scan(data s, (i-1)::BSInt);
		(a::SingleFloat, b::SingleInteger + 1);
	}
	scan(s, i): (DoubleFloat,   SingleInteger) == {
		(a: BDFlo, b) := scan(data s, (i-1)::BSInt);
		(a::DoubleFloat, b::SingleInteger + 1);
	}
	scan(s, i): (SingleInteger, SingleInteger) == {
		(a: BSInt, b) := scan(data s, (i-1)::BSInt);
		(a::SingleInteger, b::SingleInteger + 1);
	}
	scan(s, i): (Integer,       SingleInteger) == {
		(a: BBInt, b) := scan(data s, (i-1)::BSInt);
		(a::Integer, b::SingleInteger + 1);
	}
}

NumberScanPackage(
	R: with {
		*:      (%, %) -> %;
		^:      (%, Integer) -> %;
		coerce: Integer -> %;
	}
) : with {
		scanNumber : String -> R;
}
== add {
	import from String, Character;

	local bufpos : SingleInteger := 1;

	macro {
		SI                == SingleInteger;
		I                 == Integer;
		isSpaceChar c	  == c = char " ";
		isRadixChar c     == c = char "r";
		isPointChar c     == c = char ".";
		isExponChar c     == c = char "e" or c = char "E";
		isNegSignChar c   == c = char "-";
		isPosSignChar c   == c = char "+";
		isdigit c         == digit? c;
		isUpper c         == letter? c and c = upper c;
		scPeekChar()      == buf.bufpos;
		scAdvance()       == bufpos := bufpos+1;
	}

	numval(c: Character): I ==
		(ord c - ord char "0")::I;

	numCharVal(c: Character): I == {
	 	digit? c => numval c;
	 	(ord c - ord char "A")::I + 10;
	}

	scanSign(buf: String): I == {
		free bufpos;
		sign : I := 1;
		if isNegSignChar(c := scPeekChar()) then {
			sign := -1;
			scAdvance();
		}
		else if isPosSignChar(c) then {
			scAdvance();
		}
		sign;
	}

	scanInteger(buf: String): I == {
		free bufpos;
		ans : I := 0;
		while isdigit(c := scPeekChar()) repeat {
			ans := ans*10 + numval(c);
			scAdvance();
		}
		ans;
	}

	-- Note: accepts alphabetics whenever explicit radix is given
	scanRadixInteger(buf: String, radix: I, result: I) :I == {
		free bufpos;
		while digit?(c := scPeekChar()) or
		      (radix > 10 and letter?(c) and c = upper c)
		repeat {
			result := result*radix + numCharVal(c);
			scAdvance();
		}
		result;
	}

	scanNumber(buf: String): R == {
		free bufpos;
		bufpos : SingleInteger := 1;

		-- Remove leading spaces.
		while isSpaceChar(scPeekChar()) repeat scAdvance();

		-- Collect the first group of numbers.
		sign := scanSign(buf);
		mantissa := scanInteger(buf);
		radix : I := 10;

		-- Have the first group of numbers.
		if (isRadixChar(c := scPeekChar())) then {
			radix := mantissa;
			scAdvance();
			mantissa := scanRadixInteger(buf, radix, 0);
		}

		-- Have the whole part and radix, if any.
		if isPointChar(scPeekChar()) then {
			scAdvance();
			oldpos    := bufpos;
			mantissa  := scanRadixInteger(buf, radix, mantissa);
			fracexpon := oldpos - bufpos;
		}
		else
			fracexpon := 0;

		mantissa := mantissa * sign;

		-- Have the fraction part. Now collect the exponent.
		if isExponChar(scPeekChar()) then {
			scAdvance();
			sign := scanSign(buf);
		        expon := scanInteger(buf);
		        expon := expon * sign;
		        expon := expon + fracexpon::I;
		}
		else
			expon := fracexpon::I;

		-- Put it all together
		r := mantissa::R;
		if expon ~= 0 then r := r * (radix::R)^expon;
		r;
	}
}
