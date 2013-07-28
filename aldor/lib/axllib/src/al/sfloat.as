-----------------------------------------------------------------------------
----
---- sfloat.as: Single-precision floating point arithmetic.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ SingleFloat implements single-precision floating point numbers.
+++
+++ In the portable byte code files, single precision floats are represented in
+++ a format cabable of representing IEEE extended single precision.
+++ In a running program, single precision floats are represented according 
+++ to the machine's native arithmetic.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: single-precision floating point number, IEEE

SingleFloat: Join(
	OrderedFinite,
	FortranReal,
	FloatingPointNumberSystem,
	DenseStorageCategory
) with == add {
	Rep == BSFlo;

	import from Machine;
	import from RoundingMode;

	import {
		fiSFloExponent: BSFlo -> BInt;
		fiSFloMantissa: BSFlo -> BInt;
	} from Foreign C "";

	0: %       == per 0;
	1: %       == per 1;
	min: %     == per min;
	max: %     == per max;
	epsilon: % == per epsilon;
	#: Integer == (-1)@BBInt::Integer;		--!! not implemented

	float     (l: Literal): % == per convert (l pretend BArr);

	zero?	  (x: %): Boolean == zero?(rep x)::Boolean;
	negative? (x: %): Boolean == negative? (rep x)::Boolean;
	positive? (x: %): Boolean == positive? (rep x)::Boolean;

	(x: %) =  (y: %): Boolean == (rep x = rep y)::Boolean;
	(x: %) ~= (y: %): Boolean == (rep x ~= rep y)::Boolean;
	(x: %) <  (y: %): Boolean == (rep x < rep y)::Boolean;
	(x: %) >  (y: %): Boolean == (rep y < rep x)::Boolean;
	(x: %) <= (y: %): Boolean == (rep x <= rep y)::Boolean;
	(x: %) >= (y: %): Boolean == (rep y <= rep x)::Boolean;
	(x: %) << (y: %): Boolean == (rep y = rep x + rep y)::Boolean;
	(x: %) >> (y: %): Boolean == (rep x = rep x + rep y)::Boolean;

	+ (x: %): %		  == x;
	- (x: %): %		  == per (- rep x);

	(x: %) + (y: %): %	  == per (rep x + rep y);
	(x: %) - (y: %): %	  == per (rep x - rep y);
	(x: %) * (y: %): %	  == per (rep x * rep y);
	(x: %) \ (y: %): %	  == y / x;
	(x: %) / (y: %): %	  == per (rep x / rep y);

	(x: %) ^ (n: Integer): % == {
		n < 0 => inv (x^(-n));
		power(1, x, n)$BinaryPowering(%, *, Integer);
	}
	-- Exponentiation to SingleInteger powers is not provided
	-- to avoid conflicts with Integer powers.
	-- (x: %) ^ (n: SingleInteger): % == 
	--	power(1, x, n)$BinaryPowering(%, *, SingleInteger);

	inv     (x: %): %	  == 1 / x;

	gcd(x: %, y: %): %	  == 1;
	(x: %) quo (y: %): %	  == x / y;
	(x: %) rem (y: %): %	  == 0;
	divide(x: %, y: %): (%,%) == (x / y, 0);

	(w: TextWriter) << (x: %): TextWriter == {
		import from SingleInteger;
		s:String:=new(50);
                i:= format(rep x,s pretend Arr,0);
		w << s; 
		dispose! (s pretend Arr);
		w ;
		}

	step(n:SingleInteger)(a:%, b:%): Generator % == generate {
		del := (b - a)/(n - 1)::%;
		for i in 1..n repeat {
			yield a;
			a := a + del;
		}
	}

	coerce(n: Integer): %	    == per convert(n::BBInt);
	coerce(n: SingleInteger): % == per convert(n::BSInt);
	coerce(x: BSFlo): %	    == per x;
	coerce(x: %): BSFlo	    == rep x;

	mantissa(x: %): Integer == coerce fiSFloMantissa(rep x);
	exponent(x: %): Integer == coerce fiSFloExponent(rep x);

	fraction(x:%):% == per fraction(x::SFlo);
	integer(x:%):Integer  == truncate(x::SFlo)::Integer;
	round(x:%, mode:RoundingMode):Integer == 
		round(x::SFlo, mode::SInt)::Integer;

	prev(x:%):% == prev(x::BSFlo)::%;
	next(x:%):% == next(x::BSFlo)::%;

	round_+(a:%, b:%, mode:RoundingMode):% ==
		round_+(a::BSFlo, b::BSFlo, mode::SInt)::%;
	round_-(a:%, b:%, mode:RoundingMode):% ==
		round_-(a::BSFlo, b::BSFlo, mode::SInt)::%;
	round_*(a:%, b:%, mode:RoundingMode):% ==
		round_*(a::BSFlo, b::BSFlo, mode::SInt)::%;
	round_*_+(a:%, b:%, c:%, mode:RoundingMode):% ==
		round_*_+(a::BSFlo, b::BSFlo, c::BSFlo, mode::SInt)::%;
	round_/(a:%, b:%, mode:RoundingMode):% ==
		round_/(a::BSFlo, b::BSFlo, mode::SInt)::%;

	floor(x: %): % == coerce round(x, down());
	ceiling(x: %): % == coerce round(x, up());


#if ExplicitPackedExports
	-- The compiler can generate these exports automatically
	-- but could not always inline them unless they were defined
	-- explicitly.
	import
	{
			ArrNew: (Rep, BSInt) -> BArr;
			ArrElt: (BArr, BSInt) -> Rep;
			ArrSet: (BArr, BSInt, Rep) -> Rep;
	} from Builtin;

	PackedArrayNew(n:BSInt):BArr ==
		ArrNew(0, n);

	PackedArrayGet(a:BArr, i:BSInt):% ==
		per ArrElt(a, i);

	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
		per ArrSet(a, i, rep e);

	PackedRecordGet(p:BPtr):% ==
		per ArrElt(p pretend BArr, 0);

	PackedRecordSet(p:BPtr, e:%):% ==
		per ArrSet(p pretend BArr, 0, rep e);

	PackedRepSize():BSInt == {
		import {
			TypeSFlo: () -> BSInt;
			RawRepSize: BSInt -> BSInt;
		} from Builtin;

		RawRepSize(TypeSFlo());
	}
#endif
}
