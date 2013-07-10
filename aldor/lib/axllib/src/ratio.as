-----------------------------------------------------------------------------
----
---- ratio.as:  Arithmetic on fractions.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib.as"

Ratio(I: IntegerNumberSystem): Join(OrderedRing, Field) with {
        *:       (I, %) -> %;
        /:       (I, I) -> %;
        numer:   % -> I;
        denom:   % -> I;
        coerce:  I -> %;

	export from I;
}
== add {
        Rep == Record(numer: I, denom: I);

        import from Rep;

        default a, b: %;

        cancelGcd(n: I, d: I): (I, I) == {
		g := gcd(n, d);
		(n quo g, d quo g)
	}
--	(n: I) /  (d: I): % == per [cancelGcd(n,d)];

        ratio(n: I, d: I): % == per [n, d];

	gcd(a: %, b: %): % == 1;
	(a: %) quo (b: %): % == a/b;
	(a: %) rem (b: %): % == 0;
	divide(a: %, b: %): (%, %) == (a/b, 0);

        reduce(n:I, d:I): % == {
		g := gcd(n, d);
		ratio(n quo g, d quo g)
	}
        reduce(r: %): % == {
		g := gcd(numer r, denom r);
		ratio(numer r quo g, denom r quo g)
	}
        cancelGcd!(r: Rep): I == {
		g := gcd(r.numer, r.denom);
		r.numer := r.numer quo g;
		r.denom := r.denom quo g;
		g
	}
        normalize(n:I, d:I): % == {
		d < 0 => ratio(-n,-d);
		ratio(n,d)
	}
        normalize!(r: Rep): % == {
		r.denom >= 0 => per r;
		r.numer := -r.numer;
		r.denom := -r.denom;
		per r
	}

	-- Public Part --
        numer(a: %): I == rep(a).numer;
        denom(a: %): I == rep(a).denom;

        (p: TextWriter) << (z: %): TextWriter ==
                p << "(" << numer z << "/" << denom z << ")";

        (a: %) =  (b: %): Boolean == numer a = numer b and denom a = denom b;
        (a: %) ~= (b: %): Boolean == ~(a = b);
        (a: %) <  (b: %): Boolean == denom b * numer a  <  denom a * numer b;
        (a: %) <= (b: %): Boolean == denom b * numer a  <= denom a * numer b;
        (a: %) >  (b: %): Boolean == denom b * numer a  >  denom a * numer b;
        (a: %) >= (b: %): Boolean == denom b * numer a  >= denom a * numer b;
	max(a: %, b: %): %        == if a > b then a else b;
	min(a: %, b: %): %        == if a < b then a else b;
        
	sign     (a: %): %       == ratio(sign numer a, 1);
	abs      (a: %): %       == if negative? a then -a else a;
        zero?    (a: %): Boolean == zero?     numer a;
	negative?(a: %): Boolean == negative? numer a;
	positive?(a: %): Boolean == positive? numer a;
	
	coerce(n: SingleInteger): % == n::I::%;
	coerce(n: Integer): % == n::I::%;
        coerce(n: I): % == ratio(n, 1);

        inv(a: %): % == normalize(denom a, numer a);

        0: % == ratio(0, 1);
        1: % == ratio(1, 1);

	+(a: %): % == a;
        -(a: %): % == ratio(-numer a, denom a);

        (a: %) + (b: %): % == {
                z := [denom a, denom b];
                g := cancelGcd! z;
                y := [z.denom * numer a + z.numer * numer b, g];
                cancelGcd! y;
                y.denom := y.denom * z.numer * z.denom;
                normalize! y
	}
        (a: %) - (b: %): % == a + (-b);

        (a: %) * (b: %):% == {
                a1 := reduce(numer a, denom b);
                a2 := reduce(numer b, denom a);
                ratio(numer a1 * numer a2, denom a1 * denom a2)
	}
        (n: I) * (b: %): % == {
                g := gcd(n, denom b);
                ratio((n quo g) * numer b, denom b quo g)
	}
        (n: I) / (d: I): % == if d < 0 then reduce(-n,-d) else reduce(n,d);
        (a: %) / (b: %): % == a * inv b;
        (a: %) \ (b: %): % == inv a * b;

	pow(T) ==> power$BinaryPowering(Rep, mult!, T);

	mult!(aa: Rep, bb: Rep): Rep == {
		a := per aa;
		b := per bb;
                t1 := reduce(numer a, denom b);
                t2 := reduce(numer b, denom a);
                m  := ratio(numer t1 * numer t2, denom t1 * denom t2);
		aa.numer := numer m;
		aa.denom := denom m;
		aa
	}
        (a: %) ^ (n: Integer): % == 
		if n < 0 then inv (a^(-n)) else
		per pow(Integer)      ([1,1], [numer a,denom a], n);
}
