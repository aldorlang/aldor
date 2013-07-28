-----------------------------------------------------------------------------
----
---- complex.as: The algebra of complex numbers.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"


+++ Complex(R) implements complex numbers over a field, R.
+++ Domains created by the constructor are fields and implement
+++ additional operations to manipulate the real and imaginary parts
+++ of complex numbers. May be used in PackedArrays.

define ComplexField(R: Field):Category == Join(Field, DenseStorageCategory) with
{
	if (R has FortranReal) then FortranComplexReal;
	if (R has FortranDouble) then FortranComplexDouble;

        *: (R, %) -> %;
                ++ r * c is the product of the real number r and the
                ++ complex number c

        *: (%, R) -> %;
                ++ c * r is the product of the complex number c and
                ++ the real number r

	/: (%, R) -> %;
                ++ c / r is the quotient of the complex number c and
                ++ the real number r

        complex: (R,R) -> %;
                ++ complex(r, i) creates a complex number with real
                ++ part r and imaginary part i.

        real: % -> R;
                ++ real(c) returns the real part of the complex number c.

        imag: % -> R;
                ++ imag(c) returns the imaginary part of the complex
                ++ number c.

        norm: % -> R;
                ++ norm(c) computes the algebraic norm of c.
		++ This is real(c)^2 + imag(c)^2' and is the square
		++ of the norm given by abs(c). 

        conjugate: % -> %;
                ++ conjugate(c) returns the complex conjugate of the
                ++ complex number c

        %i: %;
                ++ %i is the literal constant i = complex(0,1)

	coerce: R -> %;

	copy!: (%, %) -> ();

	export from R;
}

Complex(R: Field): ComplexField(R) with == add
{
        -- The representation of complex numbers is a
        -- record of the real and imaginary parts.
        Rep == Record(real: R, imag: R);

        import from Rep;

        -- Now we implement the complex operations in the
        -- usual manner.

        complex(r: R, i: R): % == per [r, i];
        real(a: %): R          == rep(a).real;
        imag(a: %): R          == rep(a).imag;

        norm(a: %): R          == real a * real a + imag a * imag a;
        conjugate(a: %): %     == complex(real a, -imag a);

	coerce(n: SingleInteger): % == n::R::%;
	coerce(n: Integer): % == n::R::%;
	coerce(x: R): % == complex(x, 0);

        0:  % == complex(0, 0);
        1:  % == complex(1, 0);
        %i: % == complex(0, 1);

        - (a: %): % == complex(- real a, - imag a);
        + (a: %): % == a;

        (a: %) + (b: %): % == complex(real a + real b,  imag a + imag b);
        (a: %) - (b: %): % == complex(real a - real b,  imag a - imag b);
        (r: R) * (a: %): % == complex(r * real a, r * imag a);
        (a: %) * (r: R): % == complex(real(a) * r, imag(a) * r);
        (a: %) * (b: %): % ==
            complex(real a * real b - imag a * imag b,
                    real a * imag b + imag a * real b);

	inv(a: %): % == 1 / a;

	(a: %) \ (b: %): % == b / a;
	(a: %) / (r: R): % == complex(real(a) / r, imag(a) / r);

	(a: %) / (b: %): % == {
	    d: R == real b * real b  + imag b * imag b;
	    complex((real a *real b  + imag a * imag b)/d,
		    (imag a *real b  - real a * imag b)/d);
	}

	pow(T) ==> power$BinaryPowering(Rep, mult!, T);

	mult!(a: Rep, b: Rep): Rep == {
		x := a.real * b.real - a.imag * b.imag;
		y := a.real * b.imag + a.imag * b.real;
		a.real := x;
		a.imag := y;
		a
	}
        (a: %) ^ (n: Integer): % == 
		if n < 0 then inv (a^(-n)) else
		per pow(Integer)      ([1,0], [real a, imag a], n);

        (a: %) = (b: %): Boolean ==
            real a = real b and imag a = imag b;

        (a: %) ~= (b: %): Boolean == ~(a = b);
	zero?(a: %): Boolean      == a = 0;

	gcd(a: %, b: %): %    == 1;
	(a: %) quo (b: %): %  == a / b;
	(a: %) rem (b: %): %  == 0;
	divide(a: %, b: %): (%, %) == (a/b, 0);

	copy!(x:%, y:%):() ==
	{
		xp:Rep := rep x;
		yp:Rep := rep y;

		xp.real := yp.real;
		xp.imag := yp.imag;
	}

        (p: TextWriter) << (a: %): TextWriter ==
		p << "(" << real a << " + " << imag a << " %i)";

	-- Note that these operations are 0-based
	import from Machine;

	PackedArrayNew(n:BSInt):BArr ==
		PackedArrayNew(n + n)$R;

	PackedArrayGet(a:BArr, i:BSInt):% == {
		local re,im:R;

		re := PackedArrayGet(a, i + i)$R;
		im := PackedArrayGet(a, i + i + 1)$R;
		complex(re, im);
	}

	PackedArraySet(a:BArr, i:BSInt, e:%):% == {
		local re,im:R;

		re := real(e);
		im := imag(e);
		PackedArraySet(a, i + i, re)$R;
		PackedArraySet(a, i + i + 1, im)$R;
		e; -- No point in boxing
	}

	PackedRecordGet(p:BPtr):% == {
		local re,im:R;

		re := PackedArrayGet(p pretend BArr, 0)$R;
		im := PackedArrayGet(p pretend BArr, 1)$R;
		complex(re, im);
	}

	PackedRecordSet(p:BPtr, e:%):% == {
		local re,im:R;

		re := real(e);
		im := imag(e);
		PackedArraySet(p pretend BArr, 0, re)$R;
		PackedArraySet(p pretend BArr, 1, im)$R;
		e; -- No point in boxing
	}

	PackedRepSize():BSInt == {
		local rsize:BSInt := PackedRepSize()$R;

		-- We use twice the storage as R
		rsize + rsize;
	}
}
