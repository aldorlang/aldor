-----------------------------------------------------------------------------
----
---- dfloat.as: Double precision floating point arithmetic.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ DoubleFloat implements double-precision floating point numbers.
+++
+++ In the portable byte code files, double precision floats are represented in
+++ a format capable of representing IEEE extended double precision.
+++ In a running program, double precision floats are represented according 
+++ to the machine's native arithmetic.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: double-precision floating point number, IEEE

DoubleFloat: Join(
	OrderedFinite,
	FortranDouble,
	FloatingPointNumberSystem,
	DenseStorageCategory
) with {
	coerce: SingleFloat -> %;
	retract: % -> SingleFloat;
        --
        --  Added by BDS to address pack0.as bug
        --
        coerce: BDFlo -> %;
        retract: % -> BDFlo;
        --
        --  End of additions for pack0.as bug
        --
} == add {
	RawRep == BDFlo;
	Rep == Record(double: RawRep);
	SI  ==> SingleInteger;

	import from Rep, Machine;
	import from RoundingMode;

	import {
		fiDFloExponent: BDFlo -> BInt;
		fiDFloMantissa: BDFlo -> BInt;
	} from Foreign C;

	box(b) ==> per [b];
	raw(f) ==> rep(f).double;

	0:	 % == box 0;
	1:	 % == box 1;
	min:	 % == box min;
	max:	 % == box max;
	epsilon: % == box epsilon;
	#: Integer == (-1)@BBInt::Integer;	--!! not implemented

	float     (l: Literal): % == box convert(l pretend BArr);

	zero?	  (x: %): Boolean == zero?    (raw x)::Boolean;
	negative? (x: %): Boolean == negative?(raw x)::Boolean;
	positive? (x: %): Boolean == positive?(raw x)::Boolean;

	(x: %) =  (y: %): Boolean == (raw x =  raw y)::Boolean;
	(x: %) ~= (y: %): Boolean == (raw x ~= raw y)::Boolean;
	(x: %) <  (y: %): Boolean == (raw x <  raw y)::Boolean;
	(x: %) >  (y: %): Boolean == (raw y <  raw x)::Boolean;
	(x: %) <= (y: %): Boolean == (raw x <= raw y)::Boolean;
	(x: %) >= (y: %): Boolean == (raw y <= raw x)::Boolean;
	(x: %) << (y: %): Boolean == (raw y = raw x + raw y)::Boolean;
	(x: %) >> (y: %): Boolean == (raw x = raw x + raw y)::Boolean;

	+ (x: %): %		  == x;
	- (x: %): %		  == box (- raw x);
	(x: %) + (y: %): %	  == box (raw x + raw y);
	(x: %) - (y: %): %	  == box (raw x - raw y);
	(x: %) * (y: %): %	  == box (raw x * raw y);
	(x: %) \ (y: %): %	  == y / x;
	(x: %) / (y: %): %	  == box (raw x / raw y);

	mult!(x: Rep, y: Rep): Rep == {
		x.double := x.double * y.double;
		x;
	}
	(x: %)^(n: Integer): % ==
		if n < 0 then inv (x^(-n)) else
		per (power([1],[raw x],n)$BinaryPowering(Rep,mult!,Integer));

	-- Exponentiation to SingleInteger powers is not provided
	-- to avoid conflicts with Integer powers.
	-- (x: %)^(n: SI): % ==
	-- 	if n < 0 then inv (x^(-n)) else
	--	per (power([1],[raw x],n)$BinaryPowering(Rep,mult!,SI));

	inv (x: %): %	          == 1 / x;

	gcd(x: %, y: %): %	  == 1;
	(x: %) quo (y: %): %	  == x / y;
	(x: %) rem (y: %): %	  == 0;
	divide(x: %, y: %): (%,%) == (x / y, 0);

	(w: TextWriter) << (x: %): TextWriter == {
		import from Character,SingleInteger;
	        s:String:=new(50);
                i:= format(raw x,s pretend Arr,0);
		w << s; 
		dispose! (s pretend Arr);
		w ;
		}

	--!! This should be changed to be exact at the b endpoint.
	step(n:SingleInteger)(a:%, b:%): Generator % == generate {
		del := (b - a)/(n - 1)::%;
	        x := a;
		for i in 1..n repeat {
			yield x;
			x := x + del;
		}
	}
	coerce(n: Integer): %	    == box convert(n::BBInt);
	coerce(n: SingleInteger): % == box convert(n::BSInt);
	coerce(x: BDFlo): %	    == box x;
	coerce(x: %): BDFlo	    == raw x;
	coerce(x: SingleFloat): %   == (convert(x::BSFlo)@BDFlo)::%;
	retract(x:%):SingleFloat    == (convert(x::BDFlo)@BSFlo)::SingleFloat;

        --
        --  Added by BDS for pack0.as bug
        --
        retract(x:%):BDFlo    == {
                (x::BDFlo);
        }
        --
        --  End of addition for pack0.as bug
        --

	mantissa(x: %): Integer == coerce fiDFloMantissa(raw x);
	exponent(x: %): Integer == coerce fiDFloExponent(raw x);

	fraction(x:%):%    == box fraction(x::DFlo);
	integer(x:%):Integer    == truncate(x::DFlo)::Integer;
	round(x:%, mode:RoundingMode):Integer == 
		round(x::DFlo, mode::SInt)::Integer;

	prev(x:%):% == prev(x::BDFlo)::%;
	next(x:%):% == next(x::BDFlo)::%;

	round_+(a:%, b:%, mode:RoundingMode):% ==
		round_+(a::BDFlo, b::BDFlo, mode::SInt)::%;
	round_-(a:%, b:%, mode:RoundingMode):% ==
		round_-(a::BDFlo, b::BDFlo, mode::SInt)::%;
	round_*(a:%, b:%, mode:RoundingMode):% ==
		round_*(a::BDFlo, b::BDFlo, mode::SInt)::%;
	round_*_+(a:%, b:%, c:%, mode:RoundingMode):% ==
		round_*_+(a::BDFlo, b::BDFlo, c::BDFlo, mode::SInt)::%;
	round_/(a:%, b:%, mode:RoundingMode):% ==
		round_/(a::BDFlo, b::BDFlo, mode::SInt)::%;

	floor(x: %): % == coerce round(x, down());
	ceiling(x: %): % == coerce round(x, up());


	-- When we eventually have Rep == DFlo then the compiler
	-- will be able to generate these automatically just as
	-- it does for other domains such as SingleFloat.
	import
	{
		ArrNew: (RawRep, BSInt) -> BArr;
		ArrElt: (BArr, BSInt) -> RawRep;
		ArrSet: (BArr, BSInt, RawRep) -> RawRep;
	} from Builtin;

	PackedArrayNew(n:BSInt):BArr ==
		ArrNew(0, n);

	PackedArrayGet(a:BArr, i:BSInt):% ==
		box ArrElt(a, i);

	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
	{
		ArrSet(a, i, raw e);
		e; -- No point in boxing the result of ArrSet
	}

	PackedRecordGet(p:BPtr):% ==
		box ArrElt(p pretend BArr, 0);

	PackedRecordSet(p:BPtr, e:%):% == {
		ArrSet(p pretend BArr, 0, raw e);
		e;  -- No point in boxing the result of ArrSet
	}

	PackedRepSize():BSInt == {
		import {
			TypeDFlo: () -> BSInt;
			RawRepSize: BSInt -> BSInt;
		} from Builtin;

		RawRepSize(TypeDFlo());
	}
}


extend RoundingMode: BasicType with {
}  == add {
	sample: % == nearest();
}

