-----------------------------------------------------------------------------
----
---- format.as: Conversion of numbers to and from strings.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------
#include "axllib"

local Float10;

macro {
	I  == Integer;
	S  == String;
	SI == SingleInteger;
	LENGTH(aa) == length(aa)::I;
	PrintState == 'floating,fixed,general';
}



-- domain to handle state of printing
FloatPrintDefaults: with {
	export from PrintState;
	outputStyle: () -> PrintState;
	setOutputStyle!: PrintState -> ();

	outputPrecision: () -> I;
	setOutputPrecision: I -> ();
} == add {
	local state: PrintState;
	local prec:  I;
	state := general;
	prec  := -1;

	outputStyle(): PrintState == state;
	setOutputStyle!(x: PrintState): () == {
		free state;
		state := x;
	}

	outputPrecision(): I == prec;
	setOutputPrecision(i: I): () == {
		free prec;
		prec := i;
	}
}

-- idea is that this domain can handle all float domains,
-- including DoubleFloat (this should make printing pretty 
-- uniform.  One problem is that it doesn't handle IEEE 
-- oddnesses like NaN's and -0.  These should be dealt with 
-- before calling this package.
DefaultFloatPrinter(
	F: BasicType,
	mantissa: F -> I,
	exponent: F -> I,
	bits:     F -> I): with {
		convert: F -> S;
		fixed: F -> S;
		floating: F -> S;
		general: F -> S;
} == add {
	import from FloatPrintDefaults;
	import from FloatingPointToString(F, mantissa, exponent, bits);

	convert(f: F): S == {
		style := outputStyle();
		style = fixed => fixed f;
		style = floating => floating f;
		style = general => general f;
		never;
	}
	
	general(f: F):  S == dealWith general(f, outputPrecision());
	fixed(f: F):    S == dealWith fixed(f, outputPrecision());
	floating(f: F): S == dealWith floating(f, outputPrecision());

	local dealWith(int: S, frac: S, exp: S): S ==  {
		import from SingleInteger;
		if #exp ~= 0 then exp := concat("e", exp);
		concat(int, ".", frac, exp);
	}
}

-- Simple conversion operations.  NB This _should not_
-- use PrintDefaults
FloatingPointToString(F: BasicType,
			       mantissa: F -> I,
			       exponent: F -> I,
			       bits:     F -> I): with {
	fixed:      (F, I) -> (S, S, S);
	general:    (F, I) -> (S, S, S);
	floating:   (F, I) -> (S, S, S);
} == add {
	import from Float10;

	local string(l: Literal): Character == {
		import from SingleInteger, String;
		string(l).1;
	}

	convert(n: I): S == {
		import from Character;
		n = 0 => "0";
		nums: S := "0123456789";
		ln: SI := retract length10(n);
		if n<0 then ln := ln+1;
		s: S:=new(ln, fill == space);
		if n<0 then {
			s(1):="-";
			n := -n;
		}
		for i in ln.. by -1 while n ~= 0 repeat {
			s(i) := nums(retract(n rem 10)+1);
			n := n quo 10;
		}
		s
	}

	--------------------------------
	-- Output routines for Floats --
	--------------------------------
	zero: Character      := "0";
	separator: Character := " ";

	local minIndex(s: S): SI == 1;

	--  fixed : % -> S
	--  floating : % -> S
	--  general : % -> S
	-- number of digits needed to represent 'bits'-bit integers
	digits(bits: I): I == max(1,4004 * (bits-1) quo 13301);
		
	fixed(f: F, prec: I): (S, S, S) == {
		local g: Float10;
		local neg?: Boolean;
		local m, e: I;
		local frac, int: S;
		local p: SI;
		assert(prec >= -1);
		neg? := negative? mantissa f;
		p := if prec = -1 then 1 else retract prec;
		zero? mantissa f => ("0",new(p, "0"),"");
		zero? exponent f => {
			int := convert abs mantissa f;
			if neg? then int := concat("-", int);
			(int, new(p, "0"), "");
		}
		df := digits(bits f);
		g := convert(abs mantissa f, exponent f, bits f, df);
		m := mantissa g;
		e := exponent g;
		-- round g to prec digits after the decimal point
		if not (prec = -1 )
		   and -e > prec and -e < 2*df then {
			l: I := length10 m;
			g := normalize(g,l+e+prec);
			m := mantissa.g; 
			e := exponent.g;
		}
		s: S := convert(m)@S; 
		n := (#s); 
		o: SI := retract(e)+n;
		if e >= 0 then {
			int := concat(s, new(retract e, zero));
			frac := "";
		}
		else if o <= 0 then {
			frac := concat(new((-o),zero), s);
			int := "0";
		}
		else {
			frac := s(o + minIndex s .. n + minIndex s - 1);
			int := s(minIndex s .. o + minIndex s - 1);
		}
		n := #frac;
		if prec = -1 then {
			frac := rightTrim(frac,zero);
			if frac = "" then frac := "0";
		}
		else if n > p then {
			frac := frac(minIndex frac .. p + minIndex frac- 1);
		}
		else {
			frac := concat(frac, new((p-n),zero));
		}
		if neg? then int := concat("-", int);
		(int, frac, "");
	}

	floating(f: F, prec: I): (S, S, S) == {
		local neg?: Boolean;
		local int, frac: S;
		local m, e, d: I;
		local g: Float10;
		zero? mantissa f => ("0", "0", "0");
		neg? := negative? mantissa f;
		if neg? then int := "-0" else int := "0";
		zero? exponent f => {
			frac := convert abs mantissa f;
			-- ?? should deal with precision here ??
			(int, frac, convert((#frac)::I)@S);
		}

		-- base conversion to decimal rounded to the requested prec.
		d := if prec = -1 then digits(bits f) else prec;
		g := convert(abs mantissa f, exponent f, bits f, d); 
		m := mantissa g; 
		e := exponent g;
		-- I'm assuming that length10 m = # s given n > 0
		frac := convert(m)@S; 
		n := (#frac)::I; 
		o := e+n;
		(int, frac, convert(o)@S);
	}

	general(f: F, prec: I): (S, S, S) == {
		local neg?: Boolean;
		local int, frac: S;
		zero? mantissa f => ("0", "0", "");
		neg? := negative? mantissa f;
		df := digits(bits f); 
		d: I := if prec = -1 then df else prec;
		zero? exponent f => {
			d := d + 1;
			s: S := convert(abs mantissa f)@S;
			prec ~= -1 and (e: I := (#s)::I) > d => {
				(if neg? then "-0" else "0", s, convert(e)@S);
			}
			if neg? then s := concat("-", s);
			(s, "0", "");
		}

		-- base conversion to decimal rounded to requested precision
		g := convert(abs mantissa f, exponent f, bits f, d); 
		m := mantissa.g;
		e := exponent.g;

		-- I'm assuming that length10 m = # s given n > 0
		s: S := convert(m)@S; 
		n := #s; 
		o: SI := n + retract(e);

		-- Note: at least one digit is displayed after the point
		-- and trailing zeroes after the decimal point are dropped
		if o > 0 and o <= max(n,retract d) then {
			-- fixed format: add trailing zeroes before the point
			if o > n then s := concat(s, new((o-n),zero));
			frac := rightTrim(s(o + minIndex s..n + minIndex s-1),
                                          zero);
			if frac = "" then frac := "0";
			int := s(minIndex s .. o + minIndex s - 1);
			if neg? then int := concat("-", int);
			(int, frac, "");
		}
		else if o <= 0 and o >= -5 then {
			-- fixed format: up to 5 leading zeroes after the point
			if neg? then int := "-0" else int := "0";
			(int, concat(new(-o,zero), rightTrim(s,zero)), "")
		}
		else {
			-- print using E format written  0.mantissa E exponent
			frac := rightTrim(s,zero);
			if neg? then int := "-0" else int := "0";
			(int, frac, convert(e+n::I)@S);
		}
	}
}


-- handy functions dealing with a float represented in base 10.
Float10: with {
	mantissa: % -> I;
	exponent: % -> I;
		++ base 10 versions
	float10:   (I, I) -> %;
	float10:    I     -> %;
	times:     (%, %) -> %;
	chop:      (%, I) -> %;
	normalize: (%, I) -> %;
	-- these need a precision
	quotient:  (%, %, I) -> %;
	power:     (%, I, I) -> %;
	length10:	   I -> I;
	convert:   (I, I, I, I) -> %;
		++ (m, e, b, d) converts m*2^e to a float with 'd'
		++ digits of precision.
} == add {
	Rep ==> Record(mantissa: Integer, exponent: Integer);
	import from Rep;

	mantissa(x: %): I == rep(x).mantissa;
	exponent(x: %): I == rep(x).exponent;

	float10(m: I, e: I): % == per [m, e];
	float10(m: I): % == float10(m, 0);
	times (x: %, y: %): % == per [mantissa x*mantissa y, exponent x+exponent y];

	ceilLength10 (n: I): I == 146 * LENGTH n quo 485 + 1;
	floorLength10(n: I): I == 643 * LENGTH n quo 2136;

	convert(m: I, e: I, b: I, d: I): % == {
		(q,r) := divide(abs e, b);
		b := 2^b; r := 2^r;
		-- compute 2^e = b^q * r
		h: % := power(float10 b, q, d+5);
		h    := chop(times(float10 r, h),d+5);
		m10  := float10 m;
		if e < 0 then h := quotient(m10,h,d)
		else h := normalize(times(m10,h),d);
		h
	}

	-- computes length of decimal rep of n
	length10(n: I) : I == {
--
--  2006/Dec/21
--
--  BDS: This code doesn't seem to work correctly on silky.sharcnet.ca for some
--  reason.  The loop version below does, and should work on 32-bit
--  platforms as well.  There is also an alternative implementation in
--  in float.as
--
--              n := abs n;
--              ln: I := length(n)::I;
--              upper: I := 76573 * ln quo 254370;
--              lower: I := 21306 * (ln-1) quo 70777;
--              upper = lower => upper + 1;
--              n >= 10^upper => upper + 1;
--              lower + 1
--
                ndig: I := 1;
                n := abs n;
                while (n >= 10) repeat
                {
                        ndig := ndig + 1;
                        n := n quo 10;
                }
                ndig;
        }
	
	chop(x: %,p: I): % == {
		e : I := floorLength10 mantissa x - p;
		if e > 0 then x := per [mantissa x quo 10^e,exponent x+e];
		x
	}

	normalize(x: %,p: I): % == {
		ma: I := mantissa x;
		ex: I := exponent x;
		e : I := length10 ma - p;
		if e > 0 then {
			ma := ma quo 10^(e-1);
			ex := ex + e;
			local r: I;
			(ma,r) := divide(ma, 10);
			if r > 4 then {
				ma := ma + 1;
				if ma = 10^p then { ma := 1; ex := ex + p }
			}
		}
		per [ma,ex]
	}

	quotient(x: %,y: %,p: I): % == {
		ew: I := floorLength10 mantissa y -
			ceilLength10 mantissa x + p + 2;
		if ew < 0 then ew := 0;
		mw: I := (mantissa x * 10^ew) quo mantissa y;
		ew: I := exponent x - exponent y - ew;
		normalize(per [mw,ew],p)
	}

	local zero?(x: %): Boolean == mantissa x = 0;
	local one?(x: %): Boolean == mantissa x = 1 and exponent x = 0;

	power(x: %,n: I,d: I): % == {
		zero? x => per [0, 1];
		n = 0 => per [1,0];
		n = 1 => x;
		one? x => per [0, 1];
		p: I := d + LENGTH n + 1;
		e: I := n;
		y: % := per [1, 0];
		z: % := x;
		repeat {
			if odd? e then y := chop(times(y,z),p);
			if (e := e quo 2) = 0 then return y;
			z := chop(times(z,z),p);
		}
	}

}
