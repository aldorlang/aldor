-----------------------------------------------------------------------------
----
---- float.as: Arbitrary precision (big) floating point numbers.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

-- Much of this is taken from float.as (in fact all the algorithms are)
-- Eventually, float.as will use ParFloat, and this domain (or a related
-- package) will supply the special functions now defined in float.as.
-- However, better to see if ParFloat is at least slightly stable 
-- first.

macro {
	B    == Boolean;
	I    == Integer;
	SI   == SingleInteger;
	Char == Character;
	S    == String;
	PI   == Integer;
	SF   == DoubleFloat;
	N    == Integer;
}

local approxSqrt(a: I): I == {
	import from SingleInteger, Integer;
	a < 1 => 0;
	if (n := length a) > 100 then {
		-- variable precision newton iteration
		n := n quo 4;
		s := approxSqrt shift(a, -2 * n);
		s := shift(s, n);
		return ((1 + s + a quo s) quo 2)
	}
	-- initial approximation for the root is within a factor of 2
	(new, old) := (shift(1, n quo 2)@I, 1@I);
	while new ~= old repeat
		(new, old) := ((1 + new + a quo new) quo 2, new);
	new
}

local minIndex(s: S): SI == 1;
local maxIndex(s: S): SI == #s;

local shift(m: I, e: I): I == {
	not single? e =>
		throw ArithmeticError(I, 
			"[ParFloat]Not single in shift");
	shift(m, retract e);
}


local	digits(bits: I): I == max(1,4004 * (bits-1) quo 13301);
local   ParFloat;

ParFloat: with {
	-- all operations are parameterised by nbits 
	-- for uniformity. 
	-- also, all functions return not-necessarily
	-- normalized results.

	plus: (I, %, %) -> %;
	minus: (I, %, %) -> %;
	times: (I, %, %) -> %;
	dvide: (I, %, %) -> %;
	normalize: (I, %) -> %;
	sign: (I, %) -> I;
	negate: (I, %) -> %;
	zero: I -> %;
	one:  I -> %;
	zero?: %-> Boolean;
	power:   (I, %, I) -> %;
	convert:    (I, I) -> %;
	compare: (I, %, %) -> Boolean;
	greater: (I, %, %) -> Boolean;
	
	-- next and prev are a little broken at 0	
	next: (I, %) -> %;
	prev: (I, %) -> %;
	mantissa: % -> I;
	exponent: % -> I;
	float: (I, I, I) -> %;
} == add {
	Rep  ==> Record(mantissa: I, exponent: I);
	Exponent ==> 'exponent';
	Mantissa ==> 'mantissa';
	import from Mantissa, Exponent;
	import from I;

	macro LENGTH(aa) == length(aa)::I;
	shift(x: %,n: I): % == [x.mantissa,x.exponent+n];
	apply(x: %, field: Mantissa): I == { import from Rep; rep(x).mantissa }
	apply(x: %, field: Exponent): I == { import from Rep; rep(x).exponent }
	mantissa(x: %): Integer == x.mantissa;
	exponent(x: %): Integer == x.exponent;

	[man: I, exp: I]: %  == { import from Rep; per [man,exp] }
	float(bits: I, m: I, e: I): % == normalize(bits, [m,e]);
	zero(): 	 % == [0,1];
	one():	 	 % == [1,0];
	zero(i: I): % == zero();
	one(i: I):  % == one();

	zero?(a: %): 	 B == zero?(a.mantissa);
	one?(a: %): 	 B == {
		import from SingleInteger;
		if zero? a then return false;
		if a.mantissa = 1 and a.exponent = 0 then return true;
		l := length a.mantissa;
		r := a.mantissa - shift(1,l);
		-- sb. within epsilon
		if zero?(a.mantissa - r) 
		   and a.exponent = (-l)::Integer then return true;
		return false;
	}
				     
	abs(x: %): 	 % == if negative? x then negate x else x;
	negative?(x: %): B == x.mantissa < 0;
	positive?(x: %): B == x.mantissa > 0;
	negate(a: %): % == [- a.mantissa, a.exponent];
	negate(bits: I, a: %): % == negate a;
	sign(a: %): I == if a.mantissa < 0 then -1 else 1;
	sign(bits: I, a: %): I == sign a;
	normalize(bits: I, x: %): % == {
		m := x.mantissa;
		m = 0 => zero();
		e : I := LENGTH m - bits;
		-- preserve denorms and integers
		if e > 0 then normAny(bits, x) else x
	}

	local normAny(bits: I, x: %): % == {
		m := x.mantissa;
		m = 0 => zero();
		e: I := LENGTH m - bits;
		y: I := shift(m,1-e);
		if odd? y then {
			y := (if y>0 then y+1 else y-1) quo 2;
			if LENGTH y > bits then {
				y := y quo 2;
				e := e+1;
			}
		}
		else do y := y quo 2;
		[y,x.exponent+e];
	}

	plus(bits: I, x: %,y: %): % == {
		mx: I := x.mantissa; my: I := y.mantissa;
		mx = 0 => y;
		my = 0 => x;
		ex: I := x.exponent; ey: I := y.exponent;
		ex = ey => [mx+my,ex];
		de: I := ex + LENGTH mx - ey - LENGTH my;
		de > bits+1 => x;
		de < -(bits+1) => y;
		if ex < ey then
			(mx,my,ex,ey) := (my,mx,ey,ex);
		mw: I := my + shift(mx,ex-ey);
		[mw,ey];
	}	

	times (bits: I, x: %, y: %): % == times(x, y);

	times (x: %, y: %): % == 
		[x.mantissa*y.mantissa, x.exponent+y.exponent];

	dvide(bits: I, x: %,y: %): % == {
		zero? y => throw RangeError(I, 222, "divide");
		ew: I := LENGTH y.mantissa - LENGTH x.mantissa + bits + 1;
		mw: I := shift(x.mantissa,ew) quo y.mantissa;
		ew: I := x.exponent - y.exponent - ew;
		[mw,ew];
	}
	minus(bits: I, x: %, y: %): % == plus(bits, x, negate y);

	chop(x: %,p: I): % == {
		e : I := LENGTH x.mantissa - p;
		if e > 0 then x := [shift(x.mantissa,-e),x.exponent+e];
		x;
	}
	power(bits: I, x: %, n: I): % == {
		aux(bits: I, x: %,n: I): % == {
			local y: % := one(); 
			z: % := x;
			repeat {
				if odd? n then y := chop(times(y,z), bits);
				if (n := n quo 2) = 0 then return y;
				z := chop( times(z,z), bits )
			}
		}
		zero? x => {
			n = 0 => throw ParFloat;
			n < 0 => throw ParFloat;
			zero();
		}
		n = 0 => one();
		n = 1 => x;
		one? x => one();
		y := aux(bits + LENGTH n + 2, x, abs n);
		if n < 0 then y := dvide(bits, one() ,y);
		y;
	}
	
	convert(bits: I, i: I): % == [i, 0];
	order(a: %): I == LENGTH a.mantissa + a.exponent - 1;
	-- What about non-normalised numbers
	compare(bits: I, x: %, y: %): Boolean == {
		order x = order y 
		and sign(x)@I = sign(y)@I 
		and zero? minus(bits, x, y);

	}

	greater(bits: I, x: %, y: %): B == {
		zero? y => positive? x;
		zero? x => negative? y;
		negative? x and positive? y => false;
		negative? y and positive? x => true;
		order x > order y => positive? x;
		order x < order y => negative? x;
		positive? minus(bits, x, y);
	}

	next(bits: I, f: %): % == {
		if zero? exponent f then f := normAny(bits, f);
		zero? f => normalize(bits, [1, -bits]);
		mantissa f = -1 => normalize(bits,[-1, exponent f - 1]);
		normalize(bits, [f.mantissa+1, f.exponent]);
	}		

	prev(bits: I, f: %): % == {
		if zero? exponent f then f:= normAny(bits, f);
		zero? f => normalize(bits, [1, -bits]);
		mantissa f = 1 => normalize(bits,[1, exponent f - 1]);
		normalize(bits, [f.mantissa-1, f.exponent]);
	}
}

PFloat(bits: I): FloatingPointNumberSystem with {
	-- will go away
	normalize: % -> %;
	mantissa: % ->Integer;
	exponent: % -> Integer;
	bits:     % -> I;
} == add {
	Rep == ParFloat;
	import from Rep;

	mantissa(x: %):  I == mantissa rep x;	
	exponent(x: %):  I == exponent rep x;	
	bits(x: %):	 I == bits;

	normalize(x: %): % == per normalize(bits, rep x);
	(+)(a: %, b: %): % == normalize per plus(bits, rep a, rep b);
	(*)(a: %, b: %): % == normalize per times(bits, rep a, rep b);
	(/)(a: %, b: %): % == normalize per dvide(bits, rep a, rep b);

	inv(x: %): % == 1 / x;

	gcd(x: %, y: %): %   == 1;
	(x: %) quo (y: %): % == x/y;
	(x: %) rem (y: %): % == 0;
	divide(x: %, y: %): (%, %) == (x/y, 0);
	
	(p: TextWriter) << (x: %): TextWriter == {
		import from DefaultFloatPrinter(%, mantissa, exponent, bits);
		p << convert x;
	}

	0: % == per zero(bits);
	1: % == per one(bits);
	-(a: %): % == per negate(bits, rep a);
	(^)(a:%, b: Integer): % == per power(bits, rep a, b);

	coerce(i: Integer): % == per convert(bits, i);
	coerce(i: SingleInteger): % == per convert(bits, i::Integer);
	(=)(a: %, b: %): Boolean == compare(bits, rep a, rep b);
	(>)(a: %, b: %): Boolean == greater(bits, rep a, rep b);
	
	float(l: Literal): % == {
		 import from NumberScanPackage %;
		 s: String := string l;
		 scanNumber s
	}


	(f1: %) << (f2: %): Boolean == f2 >> f1;
	(a: %) >> (b: %): B == 
		zero? b or exponent rep a - exponent rep b > bits;
	
	step(n: SingleInteger)(a0: %, b: %): Generator % == generate {
	        a := a0;
		del := (b - a)/normalize coerce(n - 1);
		for i in 1..n repeat {
			yield a;
			a := a + del;
		}
	}
	fraction(x: %): % == normalize(x - truncate x);
	integer (x: %): I == shift(mantissa rep x,exponent rep x);
	truncate(x: %): % == {
		if exponent x >= 0 then return x;
		per float(bits, shift(mantissa rep x,exponent rep x),0)
	}
	round(x: %): % == {
		half: % := per float(bits, sign(bits, rep x), -1); 
		truncate(x + half)
	}

	round(x:%, mode: RoundingMode == nearest()):I == {
		mode = zero() => integer(x);
		mode = up() =>
			if x > 0 and not zero? fraction x then
				integer(x+1)
			else
				integer x;
		mode = down() =>
			if x < 0 and zero? fraction x then
				integer(x-1)
			else
				integer x;
		mode = nearest() => integer round(x);
		integer x		
	}		

	prev(x: %): % == per prev(bits, rep x);
	next(x: %): % == per next(bits, rep x);

	floor(x: %): % == if negative? x then -ceiling(-x) else truncate x;
	ceiling(x: %): % == {
		if negative? x then return (-floor(-x));
		if zero? fraction x then x else truncate x + 1;
	}

	round_+(x: %, y: %, mode: RoundingMode == nearest()): % == 
		error "round+: Not implemented";

	round_-(x: %, y: %, mode: RoundingMode == nearest()): % == 
		error "round+: Not implemented";

	round_*(x: %, y: %, mode: RoundingMode == nearest()): % == 
		error "round+: Not implemented";

	round_*_+(x: %, y: %, z: %, mode: RoundingMode == nearest()): % == 
		error "round+: Not implemented";

	round_/(x: %, y: %, mode: RoundingMode == nearest()): % == 
		error "round+: Not implemented";
}
