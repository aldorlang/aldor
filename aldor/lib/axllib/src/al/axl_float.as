-----------------------------------------------------------------------------
----
---- float.as: Arbitrary precision (big) floating point numbers.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

-- Note, this file will likely be replaced in a later version with 
-- a type which is parameterized by the precision.

#include "axllib.as"

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


+++ Float: arbitrary precision floating point arithmetic domain
+++
+++ Author: Michael Monagan
+++ Date Created:
+++   December 1987
+++ Change History:
+++   19 Jun 1990
+++   1993 Translated to AXIOM-XL.
+++
+++ Basic Operations: outputFloating, outputFixed, outputGeneral, outputSpacing,
+++   atan, convert, exp1, log2, log10, normalize, rationalApproximation,
+++   relerror, shift, / , ^
+++ Keywords: float, floating point, number
+++ Description: `Float' implements arbitrary precision floating
+++ point arithmetic.
+++ The number of significant digits of each operation can be set
+++ to an arbitrary value (the default is 20 decimal digits).
+++ The operation `float(mantissa,exponent,base)'
+++ for integer `mantissa', `exponent' specifies the number
+++ `mantissa * base ^ exponent'
+++ The underlying representation for floats is binary
+++ not decimal. The implications of this are described below.
+++
+++ The model adopted is that arithmetic operations are rounded to
+++ to nearest unit in the last place, that is, accurate to within `2^(-bits)'.
+++ Also, the elementary functions and constants are accurate to one unit 
+++ in the last place.
+++ A float is represented as a record of two integers, the mantissa
+++ and the exponent.  The base of the representation is binary, hence
+++ a `Record(m: mantissa,e: exponent)' represents the number `m * 2 ^ e'.
+++ Though it is not assumed that the underlying integers are represented
+++ with a binary base, the code will be most efficient when this is the
+++ the case (this is true in most implementations of Lisp).
+++ The decision to choose the base to be binary has some unfortunate
+++ consequences.  First, decimal numbers like 0.3 cannot be represented
+++ exactly.  Second, there is a further loss of accuracy during
+++ conversion to decimal for output.  To compensate for this, if d
+++ digits of precision are specified, `1 + ceiling(log2 d)' bits are used.
+++ Two numbers that are displayed identically may therefore be
+++ not equal.  On the other hand, a significant efficiency loss would
+++ be incurred if we chose to use a decimal base
+++ when the underlying integer base is binary.
+++
+++ Algorithms used:
+++ For the elementary functions, the general approach is to apply
+++ identities so that the taylor series can be used, and, so
+++ that it will converge within `O( sqrt n )' steps.  For example,
+++ using the identity `exp(x) = exp(x/2)^2', we can compute
+++ `exp(1/3)' to n digits of precision as follows.  We have
+++ `exp(1/3) = exp(2 ^ (-sqrt s) / 3) ^ (2 ^ sqrt s)'.
+++ The taylor series will converge in less than sqrt n steps and the
+++ exponentiation requires sqrt n multiplications for a total of
+++ `2 sqrt n' multiplications.  Assuming integer multiplication costs
+++ `O( n^2 )' the overall running time is `O( sqrt(n) n^2 )'.
+++ This approach is the best known approach for precisions up to
+++ about 10,000 digits at which point the methods of Brent
+++ which are `O( log(n) n^2 )' become competitive.  Note also that
+++ summing the terms of the taylor series for the elementary
+++ functions is done using integer operations.  This avoids the
+++ overhead of floating point operations and results in efficient
+++ code at low precisions.  This implementation makes no attempt
+++ to reuse storage, relying on the underlying system to do
+++ `garbage collection'.  I estimate that the efficiency of this
+++ package at low precisions could be improved by a factor of 2
+++ if in-place operations were available.
+++
+++ Running times: in the following, n is the number of bits of precision
+++      `*', `/', `sqrt', `pi', `exp1', `log2', `log10': `O( n^2 )'
+++      `exp', `log', `sin', `atan':  `O( sqrt(n) n^2 )'
+++ The other elementary functions are coded in terms of the ones above.

Float: FloatingPointNumberSystem with {
	pi:		() -> %;
	one?:		% -> Boolean;
	increasePrecision: I -> PI;
	decreasePrecision: I -> PI;
	bits:		() -> PI;
	bits:		PI -> PI;
	sqrt:		% -> %;
	/:		(%, I) -> %;
	^:		(%, %) -> %;
	normalize: 	% -> %;
	relerror: 	(%, %) -> I;
	log2:		() -> %;
	log10:		() -> %;
	exp1:		() -> %;
	atan:		(%,%) -> %;
	atan:		% -> %;
	acos:		% -> %;
	sin:		% -> %;
	cos:		% -> %;
	tan:		% -> %;
	log:		% -> %;
	exp:		% -> %;
	log2:		% -> %;
	log10:		% -> %;
--	convert: 	SF -> %;
	outputFloating: ()  -> ();
	outputFloating:	(N) -> ();
	outputFixed:	()  -> ();
	outputFixed:	(N) -> ();
	outputGeneral:	()  -> ();
	outputGeneral:	(N) -> ();
	outputSpacing:	(SI) -> ();
	mantissa:	% -> I;
	exponent:	% -> I;
	bits:		% -> I;
	digits:		() -> I;
	digits:		I -> I;
	step:		SingleInteger -> (%,%) -> Generator %;
	float:		Literal -> %;
	makeFloat:	Integer -> %;
	coerce:		Integer -> %;
	makeFloat:	SingleInteger -> %;
	integer: 	% -> Integer;
	fraction: 	% -> %;
	round:		(%, mode:RoundingMode == nearest()) -> I;

	export from RoundingMode;
}
== add {
	import from I, Segment I, Format;

	ISQRT ==> approxSqrt;

	macro LENGTH(aa) == length(aa)::I;

	shift(m: I, e: I): I == {
		not single? e =>
			throw ArithmeticError(%, "Not a SingleInteger in shift");
		shift(m, retract e);
	}

	Rep  == Record(mantissa: I, exponent: I);

	Exponent ==> 'exponent';
	Mantissa ==> 'mantissa';

	import from Mantissa, Exponent;

	apply(x: %, field: Mantissa): I == { import from Rep; rep(x).mantissa }
	apply(x: %, field: Exponent): I == { import from Rep; rep(x).exponent }
	[man: I, exp: I]: %             == { import from Rep; per [man,exp] }

	BASE ==> 2;
	BITS: I := 68; -- 20 digits

	StoredConstant ==> Record(precision: PI, value: %);
	UCA ==> Record(unit: %, coef: %, associate: %);
	inc ==> increasePrecision;
	dec ==> decreasePrecision;

	-- Local utility operations:
	--  times : (%,%) -> %          -- multiply x and y with no rounding
	--  itimes: (I,%) -> %          -- multiply by a small integer
	--  chop: (%,PI) -> %           -- chop x at p bits of precision
	--  dvide: (%,%) -> %           -- divide x by y with no rounding
	--  square: (%,I) -> %          -- repeated squaring with chopping
	--  power: (%,I) -> %           -- x ^ n with chopping
	--  plus: (%,%) -> %            -- addition with no rounding
	--  sub: (%,%) -> %             -- subtraction with no rounding
	--  negate: % -> %              -- negation with no rounding
	--  ceillog10base2: PI -> PI    -- rational approximation
	--  floorln2: PI -> PI          -- rational approximation

	--  atanSeries: % -> %          -- atan(x) by taylor series |x| < 1/2
	--  atanInverse: I -> %         -- atan(1/n) for n an integer > 1
	--  expInverse: I -> %          -- exp(1/n) for n an integer
	--  expSeries: % -> %           -- exp(x) by taylor series  |x| < 1/2
	--  logSeries: % -> %           -- log(x) by taylor series 1/2 < x < 2
	--  sinSeries: % -> %           -- sin(x) by taylor series |x| < 1/2
	--  cosSeries: % -> %           -- cos(x) by taylor series |x| < 1/2
	--  piRamanujan: () -> %        -- pi using Ramanujans series

	asin(x: %): % == {
		zero? x => 0;
		negative? x => -asin(-x);
		one? x => pi()/2;
		x > 1 => throw RangeError(%, x, "asin");
		inc 5; r := atan(x/sqrt(sub(1,times(x,x)))); dec 5;
		normalize r;
	}

	acos(x: %): % == {
		zero? x => pi()/2;
		negative? x => (inc 3; r := pi()-acos(-x); dec 3; normalize r);
		one? x => 0;
		x > 1 => throw RangeError(%, x, "acos");
		inc 5; r := atan(sqrt(sub(1,times(x,x)))/x); dec 5;
		normalize r;
	}

	atan(x: %,y: %): % == {
		x = 0 => {
			y > 0 => pi()/2;
			y < 0 => -pi()/2;
			0;
		}
		-- Only count on first quadrant being on principal branch.
		theta := atan abs(y/x);
		if x < 0 then theta := pi() - theta;
		if y < 0 then theta := - theta;
		theta
	}

	atan(x: %): % == {
		zero? x => 0;
		negative? x => -atan(-x);
		if x > 1 then {
			inc 4;
			r := if zero? fraction x and x < [bits(),0]
			     then atanInverse integer x
			     else atan(1/x);
			r := pi()/2 - r;
			dec 4;
			return normalize r;
		}
		-- make |x| < O(2^(-sqrt p)) < 1/2 to speed series convergence
		-- by using the formula  atan(x) = 2*atan(x/(1+sqrt(1+x^2)))
		k : I:= ISQRT(bits()-100) quo 5;
		k := max(0,2 + k + order x);
		inc(2*k);
		for i in 1..k repeat x := x/(1+sqrt(1+x*x));
		t: % := atanSeries x;
		dec(2*k);
		t := shift(t,k);
		normalize t;
	}

	atanSeries(x: %): % == {
		-- atan(x) = x (1 - x^2/3 + x^4/5 - x^6/7 + ...)  |x| < 1
		p: I := bits() + LENGTH bits() + 2;
		s: I := d: I := shift(1,p);
		y: % := times(x, x);
		t: I := m: I := -shift(y.mantissa,y.exponent+p);
		for i in 3.. by 2 while t ~= 0 repeat {
			s := s + t quo i;
			t := (m * t) quo d;
		}
		x * [s,-p];
	}

	atanInverse(n: I): % == {
		-- compute atan(1/n) for an integer n > 1
		-- atan n = 1/n - 1/n^3/3 + 1/n^5/4 - ...
		--   pi = 16 atan(1/5) - 4 atan(1/239)
		n2: I := -n*n;
		e: I := bits() + LENGTH bits() + LENGTH n + 1;
		s: I := shift(1,e) quo n;
		t: I := s quo n2;
		for k in 3.. by 2 while t ~= 0 repeat {
			s := s + t quo k;
			t := t quo n2;
		}
		normalize [s,-e];
	}

	sin(x: %): % == {
		s: I := sign x; x := abs x; p := bits(); inc 4;
		if x > [6, 0] then {
			inc p; x := 2*pi()*fraction(x/pi()/2); bits p
		}
		if x > [3, 0] then { inc p; s := -s; x := x - pi(); bits p }
		if x > [3,-1] then { inc p; x := pi() - x; dec p }
		-- make |x| < O(2^(-sqrt p)) < 1/2 to speed series convergence
		-- by using the formula sin(3*x/3) = 3 sin(x/3) - 4 sin(x/3)^3
		-- the running time is O(sqrt p M(p)) assuming |x| < 1
		k := ISQRT (bits()-100) quo 4;
		k := max(0,2 + k + order x);
		if k > 0 then (inc k; x := x / 3^k);
		r := sinSeries x;
		for i in 1..k repeat r := itimes(3,r)-shift(r^3,2);
		bits p;
		s * r
	}

	sinSeries(x: %): % == {
		-- sin(x) = x (1 - x^2/3! + x^4/5! - x^6/7! + ... |x| < 1/2
		p := bits() + LENGTH bits() + 2;
		y := times(x,x); s: I := d: I := shift(1,p);
		m: I := - shift(y.mantissa,y.exponent+p);
		t: I := m quo 6;
		for i in 4.. by 2 while t ~= 0 repeat {
			s := s + t;
			t := (m * t) quo (i*(i+1));
			t := t quo d
		}
		x * [s,-p]
	}

	cos(x: %): % == {
		s: I := 1; x := abs x; p := bits(); inc 4;
		if x > [6,0] then {
			inc p; x := 2*pi()*fraction(x/pi()/2); dec p
		}
		if x > [3,0] then { inc p; s := -s; x := x-pi(); dec p }
		if x > [1,0] then {
			-- take care of the accuracy problem near pi/2
			inc p; x := pi()/2-x; bits p; x := normalize x;
			return (s * sin x);
		}
		-- make |x| < O(2^(-sqrt p)) < 1/2 to speed series convergence
		-- by using the formula  cos(2*x/2) = 2 cos(x/2)^2 - 1
		-- the running time is O( sqrt p M(p) ) assuming |x| < 1
		k := ISQRT (bits()-100) quo 3;
		k := max(0,2 + k + order x);
		if k > 0 then (inc k; x := shift(x,-k));
		r := cosSeries x;
		for i in 1..k repeat r := shift(r*r,1)-1;
		bits p;
		s * r
	}

	cosSeries(x: %): % == {
		-- cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! + ... |x| < 1/2
		p := bits() + LENGTH bits() + 1;
		y := times(x,x);
		d: I := shift(1,p);
		s: I := d;
		m: I := -shift(y.mantissa,y.exponent+p);
		t: I := m quo 2;
		for i in 3.. by 2 while t ~= 0 repeat {
			s := s + t;
			t := (m * t) quo (i*(i+1));
			t := t quo d;
		}
		normalize [s,-p]
	}

	tan(x: %): % == {
		s: I := sign x; x := abs x; p := bits(); inc 6; 
		if x > [3, 0] then {inc p; x:=pi()*fraction(x/pi()); dec p}
		if x > [3,-1] then {inc p; x:=pi()-x; s := -s; dec p}
		if x > 1 then {c := cos x; t := sqrt(1-c*c)/c}
		else {c := sin x; t := c/sqrt(1-c*c)}
		bits p;
		s * t
	}

	P: StoredConstant := [1,[1,2]];
	pi(): % == {
		free P;
		-- We use Ramanujan's identity to compute pi.
		-- The running time is quadratic in the precision.
		-- This is about twice as fast as Machin's identity on Lisp/VM
		--   pi = 16 atan(1/5) - 4 atan(1/239)
		bits() <= P.precision => normalize P.value;
		(P := [bits(), piRamanujan()]) value;
	}

	piRamanujan(): % == {
		-- Ramanujans identity for 1/pi
		-- Reference: Shanks and Wrench, Math Comp, 1962
		-- "Calculation of pi to 100,000 Decimals".
		n := bits() + LENGTH bits() + 11;
		t: I := shift(1,n) quo 882;
		d: I := 4*882^2;
		s: I := 0;
		for i in 2.. by 2 for j in 1123.. by 21460 while t ~= 0 repeat{
			s := s + j*t;
			m := -(i-1)*(2*i-1)*(2*i-3);
			t := (m*t) quo (d*i^3);
		}
		1/[s,-n-2];
	}

	sinh(x: %): % == {
		zero? x => 0;
		lost: I := max(- order x,0);
		2*lost > bits() => x;
		inc(5+lost); e: % := exp x; s: % := (e-1/e)/2; dec(5+lost);
		normalize s;
	}

	cosh(x: %): % == {
		inc 5; e: % := exp x; c: % := (e+1/e)/2; dec 5; normalize c
	}

	tanh(x: %): % == {
		zero? x => 0;
		lost: I := max(- order x,0);
		2*lost > bits() => x;
		inc(6+lost);
		e: % := exp x; e := e*e; t: % := (e-1)/(e+1);
		dec(6+lost);
		normalize t;
	}
	asinh(x: %): % == {
		p := min(0,order x);
		if zero? x or 2*p < -bits() then return x;
		inc(5-p); r := log(x+sqrt(1+x*x)); dec(5-p);
		normalize r;
	}

	acosh(x: %): % == {
		if x < 1 then throw RangeError(%, x, "acosh");
		inc 5; r := log(x+sqrt(sub(times(x,x),1))); dec 5;
		normalize r;
	}

	atanh(x: %): % == {
		if x > 1 or x < -1 then throw RangeError(%, x, "atanh");
		p := min(0,order x);
		if zero? x or 2*p < -bits() then return x;
		inc(5-p); r := log((x+1)/(1-x))/2; dec(5-p);
		normalize r;
	}

	log(x: %): % == {
		negative? x => throw  RangeError(%, x, "log");
		zero? x => throw  RangeError(%, x, "log");
		p := bits(); inc 5;
		-- apply  log(x) = n log 2 + log(x/2^n)  so that  1/2 < x < 2
		if (n := order x) < 0 then n := n+1;
		l: % := if n = 0 then 0 else {x := shift(x,-n); n * log2()}
		-- speed the series convergence by finding m and k such that
		-- | exp(m/2^k) x - 1 |  <  1 / 2 ^ O(sqrt p)
		-- write  log(exp(m/2^k) x) as m/2^k + log x
		k := ISQRT (p-100) quo 3;
		if k > 1 then {
			k := max(1,k+order(x-1));
			inc k;
			ek: % := expInverse (2^k);
			dec(p quo 2); m := order square(x,k); inc(p quo 2);
			m := (6847196937 * m) quo 9878417065;   -- m := m log 2
			x := x * ek ^ (-m);
			l := l + [m,-k];
		}
		l := l + logSeries x;
		bits p;
		normalize l;
	}

	logSeries(x: %): % == {
		-- log(x) = 2 y (1 + y^2/3 + y^4/5 ...)  for  y = (x-1)/(x+1)
		-- given 1/2 < x < 2 on input we have -1/3 < y < 1/3
		p := bits() + (g: I := LENGTH bits() + 3);
		inc g; y := (x-1)/(x+1); dec g;
		s: I := d: I := shift(1,p);
		z := times(y,y);
		t: I := shift(z.mantissa,z.exponent+p);
		m: I := t;
		for i in 3.. by 2 while t ~= 0 repeat {
			s := s + t quo i;
			t := m * t quo d;
		}
		y * [s,1-p];
	}

	L2: StoredConstant := [1,[1,0]];
	log2(): % == {
		--  log x  =  2 * sum( ((x-1)/(x+1))^(2*k+1)/(2*k+1), k=1.. )
		--  log 2  =  2 * sum( 1/9^k / (2*k+1), k=0..n ) / 3
		free L2;
		n := bits();
		n <= L2.precision => normalize L2.value;
		n := n + LENGTH n + 3;  -- guard bits
		s: I := shift(1,n+1) quo 3;
		t: I := s quo 9;
		for k in 3.. by 2 while t ~= 0 repeat {
			s := s + t quo k;
			t := t quo 9;
		}
		L2 := [bits(),[s,-n]];
		normalize L2.value;
	}

	L10: StoredConstant := [1,[1,1]];
	log10(): % == {
		--  log x  =  2 * sum( ((x-1)/(x+1))^(2*k+1)/(2*k+1), k=0.. )
		--  log 5/4  =  2 * sum( 1/81^k / (2*k+1), k=0.. ) / 9
		free L10;
		n := bits();
		n <= L10.precision => normalize L10.value;
		n := n + LENGTH n + 5;  -- guard bits
		s: I := shift(1,n+1) quo 9;
		t: I := s quo 81;
		for k in 3.. by 2 while t ~= 0 repeat {
			s := s + t quo k;
			t := t quo 81;
		}
		-- We have log 10 = log 5 + log 2 and log 5/4 = log 5 - 2 log 2
		inc 2; L10 := [bits(),[s,-n] + 3*log2()]; dec 2;
		normalize L10.value;
	}

	log2(x: %): % ==  { inc 2; r := log(x)/log2();  dec 2; normalize r }
	log10(x: %): % == { inc 2; r := log(x)/log10(); dec 2; normalize r }

	exp(x: %): % == {
		-- exp(n+x) = exp(1)^n exp(x) for n such that |x| < 1
		p := bits(); inc 5; e1: % := 1;
		if (n := integer x) ~= 0 then {
			inc LENGTH n; e1 := exp1() ^ n; dec LENGTH n;
			x := fraction x
		}
		if zero? x then { bits p; return normalize e1 }
		-- make |x| < O(2^(-sqrt p)) < 1/2 to speed series convergence
		-- by repeated use of the formula exp(2*x/2) = exp(x/2)^2
		-- results in an overall running time of O( sqrt p M(p) )
		k := ISQRT (p-100) quo 3;
		k := max(0,2 + k + order x);
		if k > 0 then { inc k; x := shift(x,-k) }
		e: % := expSeries x;
		if k > 0 then e := square(e,k);
		bits p;
		e * e1
	}

	expSeries(x: %): % == {
		-- exp(x) = 1 + x + x^2/2 + ... + x^i/i!  valid for all x
		p := bits() + LENGTH bits() + 1;
		s: I := d: I := shift(1,p);
		t: I := n: I := shift(x.mantissa,x.exponent+p);
		for i in 2.. while t ~= 0 repeat {
			s := s + t;
			t := (n * t) quo i;
			t := t quo d;
		}
		normalize [s,-p];
	}
	expInverse(k: I): % == {
		-- computes exp(1/k) via continued fraction
		p0: I := 2*k+1; p1: I := 6*k*p0+1;
		q0: I := 2*k-1; q1: I := 6*k*q0+1;
		for i in 10*k.. by 4*k while 2 * LENGTH p0 < bits() repeat {
			(p0,p1) := (p1,i*p1+p0);
			(q0,q1) := (q1,i*q1+q0);
		}
		dvide([p1,0],[q1,0]);
	}
	E: StoredConstant := [1,[1,1]];
	exp1(): % == {
		  free E;
		  if bits() > E.precision then E := [bits(),expInverse 1];
		  normalize E.value;
	}
	sqrt(x: %): % == {
		negative? x => throw RangeError(%, x, "sqrt");
		m := x.mantissa; e: I := x.exponent;
		l: I := LENGTH m;
		p := 2 * bits() - l + 2;
		if odd?(e-l) then p := p - 1;
		i := shift(x.mantissa,p);
		-- ISQRT uses a variable precision newton iteration
		i := ISQRT i;
		normalize [i,(e-p) quo 2];
	}
	bits(): I == BITS;
	bits(n: I): I == {free BITS; t: I := bits(); BITS := n; t}
	precision(): I == bits();
	precision(n: I): I == bits(n);
	increasePrecision(n: I): PI == {b: I := bits(); bits(b + n); b}
	decreasePrecision(n: I): PI == {b: I := bits(); bits(b - n); b}
	ceillog10base2(n: I): I == ((13301 * n + 4003) quo 4004);
	digits(): I == max(1,4004 * (bits()-1) quo 13301);
	digits(n: I): I == (t: I := digits(); bits (1 + ceillog10base2 n); t);

	order(a: %): I == LENGTH a.mantissa + a.exponent - 1;
	relerror(a: %,b: %): I == order((a-b)/b);
	0: % == [0,0];
	1: % == [1,0];
	base(): I == BASE;
	mantissa(x: %): I == x.mantissa;
	exponent(x: %): I == x.exponent;
	bits(x: %):	I == bits();
	one? (a: %): B     == a = 1;
	zero?(a: %): B     == zero?(a.mantissa);
	negative?(a: %): B == negative?(a.mantissa);
	positive?(a: %): B == positive?(a.mantissa);

	chop(x: %,p: I): % == {
		e : I := LENGTH x.mantissa - p;
		if e > 0 then x := [shift(x.mantissa,-e),x.exponent+e];
		x;
	}
	float(m: I,e: I): % == normalize [m,e];
	float(m: I,e: I,base: I): % == {
		m = 0 => 0;
		inc 4; r := m * [base,0] ^ e; dec 4;
		normalize r;
	}
	normalize(x: %): % == {
		m := x.mantissa;
		m = 0 => 0;
		e : I := LENGTH m - bits();
		if e > 0 then {
			y: I := shift(m,1-e);
			if odd? y then {
				y := (if y>0 then y+1 else y-1) quo 2;
				if LENGTH y > bits() then {
					y := y quo 2;
					e := e+1;
				}
			}
			else do y := y quo 2;
			x := [y,x.exponent+e];
		}
		x
	}
	shift(x: %,n: I): % == [x.mantissa,x.exponent+n];

	(x: %) = (y: %): B ==
		order x = order y and sign(x)@I = sign(y)@I and zero? (x - y);
	(x: %) > (y: %): B == {
		zero? y => positive? x;
		zero? x => negative? y;
		negative? x and positive? y => false;
		negative? y and positive? x => true;
		order x > order y => positive? x;
		order x < order y => negative? x;
		positive? (x-y);
	}

	abs(x: %): % == if negative? x then -x else normalize x;
	ceiling(x: %): % == {
		if negative? x then return (-floor(-x));
		if zero? fraction x then x else truncate x + 1;
	}
	integer (x: %): I == shift(x.mantissa,x.exponent);
	fraction(x: %): % == x - truncate x;
	round(x:%, mode: RoundingMode == nearest()):I == {

		mode = zero() =>	integer(x);
		mode = up() =>
			if x > 0 and fraction x ~= 0 then
				integer(x+1)
			else
				integer x;
		
		mode = down() =>
			if x < 0 and fraction x ~= 0 then
				integer(x-1)
			else
				integer x;

		mode = nearest() => {
			frac := fraction x;
			delta := {
				frac < -0.5 => -1.0;
				frac >  0.5 =>  1.0;
				0.0;
			}
			integer(x + delta)
		}

		integer x		
	}		

	floor(x: %): % == if negative? x then -ceiling(-x) else truncate x;
	round(x: %): % == {half: % := [sign x,-1]; truncate(x + half)}
	sign(x: %): I == if x.mantissa < 0 then (-1) else 1@I;
	truncate(x: %): % == {
		if x.exponent >= 0 then return x;
		normalize [shift(x.mantissa,x.exponent),0]
	}
--	recip(x) == if x=0 then "failed" else 1/x;
	differentiate(x: %): % == 0;

	- (x: %): % == normalize negate x;
	negate(x: %): % == [-x.mantissa,x.exponent];
	(x: %) + (y: %): % == normalize plus(x,y);
	(x: %) - (y: %): % == normalize plus(x,negate y);
	sub(x: %,y: %): % == plus(x,negate y);
	plus(x: %,y: %): % == {
		mx: I := x.mantissa; my: I := y.mantissa;
		mx = 0 => y;
		my = 0 => x;
		ex: I := x.exponent; ey: I := y.exponent;
		ex = ey => [mx+my,ex];
		de: I := ex + LENGTH mx - ey - LENGTH my;
		de > bits()+1 => x;
		de < -(bits()+1) => y;
		if ex < ey then
			(mx,my,ex,ey) := (my,mx,ey,ex);
		mw: I := my + shift(mx,ex-ey);
		[mw,ey];
	}

	(x: %) * (y: %): % == normalize times (x,y);
	(n: I) * (y: %): % == {
		if LENGTH n > bits() then normalize [n,0] * y;
		else normalize [n * y.mantissa,y.exponent]
	}
	(x: %) \ (y: %): % == y / x;
	(x: %) / (y: %): % == normalize dvide(x,y);
	(x: %) / (n: I): % ==
		if LENGTH n > bits() then x/normalize [n,0] else x/[n,0];
	inv(x: %): % == 1 / x;

	gcd(x: %, y: %): %   == 1;
	(x: %) quo (y: %): % == x/y;
	(x: %) rem (y: %): % == 0;
	divide(x: %, y: %): (%, %) == (x/y, 0);

	times (x: %, y: %): % == [x.mantissa*y.mantissa, x.exponent+y.exponent];
	itimes(n: I, y: %): % == [n * y.mantissa,y.exponent];

	dvide(x: %,y: %): % == {
		zero? y => throw RangeError(%, y, "divide");
		ew: I := LENGTH y.mantissa - LENGTH x.mantissa + bits() + 1;
		mw: I := shift(x.mantissa,ew) quo y.mantissa;
		ew: I := x.exponent - y.exponent - ew;
		[mw,ew];
	}

	square(x: %,n: I): % == {
		ma: I := x.mantissa; ex: I := x.exponent;
		for k in 1..n repeat {
			ma := ma * ma; ex := ex + ex;
			l: I := bits() - LENGTH ma;
			ma := shift(ma,l); ex := ex - l;
		}
		[ma,ex];
	}

	power(x: %,n: I): % == {
		y: % := 1; z: % := x;
		repeat {
			if odd? n then y := chop(times(y,z), bits());
			if (n := n quo 2) = 0 then return y;
			z := chop( times(z,z), bits() )
		}
	}

	(x: %) ^ (y: %): % == {
		x = 0 => {
			y = 0 => throw ArithmeticError(%, "0^0 is undefined");
			y < 0 => throw RangeError(%, y, "^");
			0;
		}
		y = 0 => 1;
		y = 1 => x;
		x = 1 => 1;
		p := abs order y + 5;
		inc p; r := exp(y*log(x)); dec p;
		normalize r;
	}

	(x: %) ^ (n: I): % == {
		x = 0 => {
			n = 0 => throw ArithmeticError(%, "0^0 is undefined");
			n < 0 => throw ArithmeticError(%, "division by zero");
			0;
		}
		n = 0 => 1;
		n = 1 => x;
		x = 1 => 1;
		p := bits();
		bits(p + LENGTH n + 2);
		y := power(x,abs n);
		if n < 0 then y := dvide(1,y);
		bits p;
		normalize y;
	}

	-- Utility routines for conversion to decimal
	--  ceilLength10: I -> I
	--  chop10: (%,I) -> %
	--  convert10:(%,I) -> %
	--  floorLength10: I -> I
	--  length10: I -> I
	--  normalize10: (%,I) -> %
	--  quotient10: (%,%,I) -> %
	--  power10: (%,I,I) -> %
	--  times10: (%,%,I) -> %

	convert10(x: %,d: I): % == {
		m: I := x.mantissa; e: I := x.exponent;
		--!! deal with bits here
		b: I := bits();
		(q,r) := divide(abs e, b);
		b := 2^b; r := 2^r;
		-- compute 2^e = b^q * r
		h := power10([b,0],q,d+5);
		h := chop10([r*h.mantissa,h.exponent],d+5);
		if e < 0 then h:=quotient10([m,0],h,d)
		else h:=times10([m,0],h,d);
		h
	}

	ceilLength10 (n: I): I == 146 * LENGTH n quo 485 + 1;
	floorLength10(n: I): I == 643 * LENGTH n quo 2136;

	-- computes length of decimal rep of n
	length10(n: I) : I == {
		n := abs n;
		ln: I := length(n)::I;
		upper: I := 76573 * ln quo 254370;
		lower: I := 21306 * (ln-1) quo 70777;
		upper = lower => upper + 1;
		n >= 10^upper => upper + 1;
		lower + 1
	}

	convert(n: I): S == {
		import from Character;
		n = 0 => "0";
		nums: S := "0123456789";
		ln: SI := retract length10(n);
		if n<0 then ln := ln+1;
		s: S:=new(ln, fill == space);
		if n<0 then {
			s(1):="-".1;
			n := -n;
		}
		for i in ln.. by -1 while n ~= 0 repeat {
			s(i) := nums(retract(n rem 10)+1);
			n := n quo 10;
		}
		s
	}
	chop10(x: %,p: I): % == {
		e : I := floorLength10 x.mantissa - p;
		if e > 0 then x := [x.mantissa quo 10^e,x.exponent+e];
		x
	}
	normalize10(x: %,p: I): % == {
		ma: I := x.mantissa;
		ex: I := x.exponent;
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
		[ma,ex]
	}
	times10(x: %,y: %,p: I): % == normalize10(times(x,y),p);
	quotient10(x: %,y: %,p: I): % == {
		ew: I := floorLength10 y.mantissa -
			ceilLength10 x.mantissa + p + 2;
		if ew < 0 then ew := 0;
		mw: I := (x.mantissa * 10^ew) quo y.mantissa;
		ew: I := x.exponent - y.exponent - ew;
		normalize10([mw,ew],p)
	}
	power10(x: %,n: I,d: I): % == {
		x = 0 => 0;
		n = 0 => 1;
		n = 1 => x;
		x = 1 => 1;
		p: I := d + LENGTH n + 1;
		e: I := n;
		y: % := 1;
		z: % := x;
		repeat {
			if odd? e then y := chop10(times(y,z),p);
			if (e := e quo 2) = 0 then return y;
			z := chop10(times(z,z),p);
		}
	}

	--------------------------------
	-- Output routines for Floats --
	--------------------------------
	zero      := apply("0",1$SI);
	separator := apply(" ",1$SI);

	SPACING:= (((): SI +-> 10) ());
	OUTMODE: S := "general";
	OUTPREC: I := (-1);

	--  fixed : % -> S
	--  floating : % -> S
	--  general : % -> S

	padFromLeft(s: S): S == {
		zero? SPACING => s;
		n: SI := (#s) - 1;
		t: S := new(n + 1 + n quo SPACING, separator);
		j: SI := minIndex t;
		for i: SI in 0..n repeat {
			t.j := s.(i + minIndex s);
			if (i+1) rem SPACING = 0 then j := j+1;
			j := j+1;
		}
		t
	}
	padFromRight(s: S): S == {
		zero? SPACING => s;
		n: SI := (#s) - 1;
		t: S := new(n + 1 + n quo SPACING, separator);
		j: SI := maxIndex t;
		for i: SI in n..0 by -1 repeat {
			t.j := s.(i + minIndex s);
			if (n-i+1) rem SPACING = 0 then j := j-1;
			j := j-1;
		}
		t
	}
	
	import from FloatingPointToString(%, mantissa, exponent, bits);
	
	fixed(f: %): S == {
		import from SI;
		(int, frac, exp) := fixed(f, OUTPREC);
		assert(#exp = 0);
		concat(padFromRight(int), ".", padFromLeft(frac));
	}

	floating(f: %): S == {
		(int, frac, exp) := floating(f, OUTPREC);
		t: S := if zero?(SPACING::I) then "E" else " E ";
		concat(padFromRight int, ".", padFromLeft frac, t, exp);
	}

	general(f: %): S == {
		import from SI;
		(int, frac, exp) := general(f, OUTPREC);
		if #exp ~= 0 then {
			t := if zero?(SPACING::I) then "E" else " E ";
			exp := concat(t, exp);
		}
		concat(padFromRight int, ".", padFromLeft frac, exp);
	}

	outputSpacing(n: SI): () ==
		{ free SPACING; SPACING := n }
	outputFixed(): () ==
		{ free OUTMODE, OUTPREC; OUTMODE := "fixed"; OUTPREC := -1 }
	outputFixed(n: I): () ==
		{ free OUTMODE, OUTPREC; OUTMODE := "fixed"; OUTPREC := n }
	outputGeneral(): () ==
		{ free OUTMODE, OUTPREC; OUTMODE := "general"; OUTPREC := -1 }
	outputGeneral(n: I): () == 
		{ free OUTMODE, OUTPREC; OUTMODE := "general"; OUTPREC := n }
	outputFloating(): () ==
		{ free OUTMODE, OUTPREC; OUTMODE := "floating"; OUTPREC := -1 }
	outputFloating(n: I): () == 
		{ free OUTMODE, OUTPREC; OUTMODE := "floating"; OUTPREC := n }

	convert(f: %): S == {
		b: Integer := {
			(OUTPREC = -1) and (not(zero? f)) =>
				bits(length(abs mantissa f)::I);
			0
		}
		s: S := {
			OUTMODE = "fixed"    => fixed f;
			OUTMODE = "floating" => floating f;
			OUTMODE = "general"  => general f;
			""
		}
		if b > 0 then bits(b);
		s = "" => error "bad output mode";
		s
	}

	(p: TextWriter) << (x: %): TextWriter ==
	    p << convert(x)@String;


	step(n: SingleInteger)(a: %, b: %): Generator % == generate {
		del := (b - a)/makeFloat(n - 1);
		for i in 1..n repeat {
			yield a;
			a := a + del;
		}
	}

	float(l: Literal): % == {
		 import from NumberScanPackage Float;
		 s: String := string l;
		 scanNumber s pretend %
	}

	makeFloat(i: Integer): % == normalize [i,0];
	makeFloat(si: SingleInteger): % == makeFloat(si::I);

	coerce(i: Integer): % == makeFloat(i);
	coerce(i: SingleInteger): % == makeFloat(i);

	(f1: %) << (f2: %): Boolean == f2 >> f1;
	(f1: %) >> (f2: %): Boolean == {
		zero? f2 or f1.exponent - f2.exponent > BITS;
	}

	prev(f: %): % == {
		zero? f => float(1, bits());
		zero? exponent f => {
			g := shift(f, bits()+1);
			g := prev g;
			shift(g, -bits()-1);
		}
		float(f.mantissa-1, f.exponent);
	}
					
	next(f: %): % == {
		zero? f => float(1, bits());
		zero? exponent f => {
			g :=shift(f, bits());
			g := next g;
			shift(g, -bits());
		}
		float(f.mantissa-1, f.exponent);
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

#if 0
-- this junk is now in fprint.as
	fixed(f: %): S == {
		zero? f => "0.0";
		zero? exponent f =>
			padFromRight concat(convert(mantissa f)@S, ".0");
		negative? f => concat("-", fixed abs f);
		d := if OUTPREC = -1 then digits() else OUTPREC;
		g := convert10(abs f,digits());
		m := g.mantissa;
		e := g.exponent;
		if OUTPREC ~= -1 then {
			-- round g to OUTPREC digits after the decimal point
			l: I := length10 m;
			if -e > OUTPREC and -e < 2*digits() then {
				g := normalize10(g,l+e+OUTPREC);
				m := g.mantissa; e := g.exponent;
			}
		}
		s: S := convert(m)@S; n := (#s); o: SI := retract(e)+n;
		p := if OUTPREC = -1 then n else retract OUTPREC;
		if e >= 0 then {
			s := concat(s, new(retract e, zero));
			t := "";
		}
		else if o <= 0 then {
			t := concat(new((-o),zero), s);
			s := "0";
		}
		else {
			t := s(o + minIndex s .. n + minIndex s - 1);
			s := s(minIndex s .. o + minIndex s - 1);
		}
		n := #t;
		if OUTPREC = -1 then {
			t := rightTrim(t,zero);
			if t = "" then t := "0";
		}
		else if n > p then {
			t := t(minIndex t .. p + minIndex t- 1);
		}
		else {
			t := concat(t, new((p-n),zero));
		}
		concat(padFromRight s, concat(".", padFromLeft t));
	}

	floating(f: %): S == {
		zero? f => "0.0";
		negative? f => concat("-", floating abs f);
		t: S := if zero?(SPACING::I) then "E" else " E ";
		zero? exponent f => {
			s: S := convert(mantissa f)@S;
			concat("0.", padFromLeft s, t, convert((#s)::I)@S);
		}

		-- base conversion to decimal rounded to the requested prec.
		d: I := if OUTPREC = -1 then digits() else OUTPREC;
		g: % := convert10(f,d); m := g.mantissa; e := g.exponent;

		-- I'm assuming that length10 m = # s given n > 0
		s := convert(m)@S; n := (#s)::I; o := e+n;
		s := padFromLeft s;
		concat("0.", s, t, convert(o)@S);
	}

	general(f: %): S == {
		zero? f => "0.0";
		negative? f => concat("-", general abs f);
		d: I := if OUTPREC = -1 then digits() else OUTPREC;
		zero? exponent f => {
			d := d + 1;
			s: S := convert(mantissa f)@S;
			OUTPREC ~= -1 and (e: I := (#s)::I) > d => {
				t: S := if zero? SPACING then "E" else " E ";
				concat("0.", padFromLeft s, t, convert(e)@S);
			}
			padFromRight concat(s, ".0");
		}

		-- base conversion to decimal rounded to requested precision
		g: % := convert10(f,d); m := g.mantissa; e := g.exponent;

		-- I'm assuming that length10 m = # s given n > 0
		s: S := convert(m)@S; n := #s; o: SI := n + retract(e);

		-- Note: at least one digit is displayed after the point
		-- and trailing zeroes after the decimal point are dropped
		if o > 0 and o <= max(n,retract d) then {
			-- fixed format: add trailing zeroes before the point
			if o > n then s := concat(s, new((o-n),zero));
			t := rightTrim(s(o + minIndex s..n + minIndex s-1),
                                       zero);
			if t = "" then t := "0" else t := padFromLeft t;
			s := padFromRight s(minIndex s .. o + minIndex s - 1);
			concat(s, ".", t);
		}
		else if o <= 0 and o >= -5 then {
			-- fixed format: up to 5 leading zeroes after the point
			concat("0.",
                               padFromLeft concat(new(-o,zero),
                                                  rightTrim(s,zero)));
		}
		else {
			-- print using E format written  0.mantissa E exponent
			t := padFromLeft rightTrim(s,zero);
			s := if zero? SPACING then "E" else " E ";
			concat("0.", t, s, convert(e+n::I)@S);
		}
	}
#endif
