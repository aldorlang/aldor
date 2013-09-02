----------------------------- sit_dup.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	I == MachineInteger;
	Z == Integer;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{DenseUnivariatePolynomial}
\History{Manuel Bronstein}{5/8/94}{created}
\History{Thom Mulders}{27/5/97}{added partial add!}
\Usage{ import from \this~R\\ import from \this(R, x) }
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em x} & \altype{Symbol} & The variable name (optional)\\
}
\Descr{\this(R, x) implements dense univariate polynomials with coefficients
in R.}
\begin{exports}
\category{\altype{UnivariatePolynomialAlgebra} R}\\
\end{exports}
#endif

DenseUnivariatePolynomial(R:Join(ArithmeticType, ExpressionType),
	avar:Symbol == new()): UnivariatePolynomialAlgebra R == add {
	macro Coeffs == ARR R;     -- sorted, lowest first
	-- the size indicates the allocated size of the array, we always
	-- have deg < siz but not necessarily equality
	-- siz is used to determine when to resize the array
	Rep == Record(siz:I, deg:I, koeffs:Coeffs);

	import from Coeffs, Rep, R, I;

	0:%				== { import from Z; term(0, 0); }
	1:%				== { import from Z; term(1, 0); }
	local intdom?:Boolean		== R has IntegralDomain;
	local degr(p:%):I		== rep(p).deg;
	local coeffs(p:%):Coeffs	== rep(p).koeffs;
	local setDegree!(p:%, n:I):I	== { assert(n >= 0); rep(p).deg := n; }
	-- TEMPORARY: CAUSES SEG FAULT IF DECLARED LOCAL! (BUGxxxx) 1.1.12p2
	-- local new1(n:I):Coeffs		== new next n;
	new1(n:I):Coeffs		== new next n;
	local size(p:%):I		== rep(p).siz;
	extree(p:%):ExpressionTree	== p extree avar;
	local charTimes(n:I):I		== machine(characteristic$% * (n::Z));
	(p:%) + (q:%):%			== addsub(p, q, true);
	(p:%) - (q:%):%			== addsub(p, q, false);
	(port:TextWriter) << (p:%):TextWriter   == port(p, avar);
	local poly(s:I, d:I, a:Coeffs):%	== per [s, d, a];

	truncate!(p:%, n:Z):% == {
		assert(n >= 0);
		zero? n => 0;
		zero? p or n > degree p => p;
		(d := computeDegree(prev machine n, coeffs p)) < 0 => 0;
		setDegree!(p, d);
		p;
	}

	truncate(p:%, n:Z):% == {
		assert(n >= 0);
		zero? n => 0;
		zero? p or n > degree p => p;
		a:Coeffs := new1(d := prev machine n);
		c := coeffs p;
		for i in 0..d repeat a.i := c.i;
		(m := computeDegree(d, a)) < 0 => 0;
		poly(next d, m, a);
	}

	map!(f:R -> R)(p:%):% == {
		zero? p => p;
		a := coeffs p;
		d := degr p;
		for i in 0..d repeat a.i := f(a.i);
		(deg := computeDegree(d, a)) < 0 => 0;
		setDegree!(p, d);
		p;
	}

	leadingTerm(p:%):(R, Z) == {
		zero? p => (0, -1);
		d := degr(p)::Z;	-- Using degree causes an infinite loop!
		(coefficient(p, d), d);
	}

	revert!(p:%):% == {
		zero? p => p;
		a := coeffs p;
		i:I := 0;
		j := degr p;
		while i < j repeat {
			t := a.i;
			a.i := a.j;
			a.j := t;
			i := next i;
			j := prev j;
		}
		p;
	}

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	zero?(p:%):Boolean	== zero?(degr p) and zero?(coeffs(p).0);
	one?(p:%):Boolean	== zero?(degr p) and one?(coeffs(p).0);

	local kara?:Boolean ==
		R has CommutativeRing and
		    (cutoff(CUTOFF__KARAMULT)$R >= 0);

	-- local fft?:Boolean ==
		-- R has FFTRing and
			-- (cutoff(CUTOFF__FFTMULT)$R >= 0);

	(p:%) * (q:%):% == {
		import from R;
		zero? p or zero? q => 0;
		one? p => q; one? q => p;
		-- fft? => fftProduct(p, q);
		kara? => kara(p, q);
		naiveProduct(p, q);
	}

	shift!(a:%, n:Z):% == {
		import from I;
		zero? n => a;
		(m := machine n) < 0 => shiftright!(a, -m);
		(d := degr a) + m >= size a => shift(a, n);-- TEMPO: USE resize!
		c := coeffs a;
		for i in d..0 by -1 repeat c(i+m) := c.i;
		for i in 0..prev m repeat c.i := 0;
		setDegree!(a, d + m);
		a;
	}

	local shiftright!(a:%, s:I):% == {
		zero? a or zero? s => a;
		c := coeffs a;
		d := degr a;
		for i in 0..d-s repeat c.i := c(i+s);
		setDegree!(a, d - s);
		a;
	}

	if R has RationalRootRing then {
		if R has GcdDomain then {
			macro RXY == SparseUnivariatePolynomial %;
			macro SPREAD == UnivariatePolynomialSpread(R, %, RXY);

			integerDistances(p:%):List Z ==
				integerDistances(p)$SPREAD;

			integerDistances(p:%, q:%):List Z ==
				integerDistances(p, q)$SPREAD;
		}
	}

	if R has CommutativeRing then {
		local karaCutoff:I	== cutoff(CUTOFF__KARAMULT)$R;
		local nokara?:Boolean	== zero? karaCutoff;
		local kara(p:%, q:%):%	== karatsuba(p, q, karaCutoff);

#if WAIT_FOR_FFT
		if R has FFTRing then {
			local fftCut:I		== cutoff(CUTOFF__FFTMULT)$R;
			local fftprod:(%, %) -> Partial %	== fft(%)$R;

			local fftProduct(p:%, q:%):% == {
				assert(fftCut > 0);
				import from Partial %;
				min(degr p, degr q) < fftCut or
					failed?(u := fftprod(p, q)) => {
						nokara? => naiveProduct(p, q);
						karatsuba(p, q, karaCutoff);
				}
				retract u;
			}
		}
#endif

		-- EXPORT THIS FUNCTION FOR THE PURPOSE OF TUNING THE
		-- KARATSUBA CUTOFFs FOR VARIOUS COEFF RINGS
		local karatsuba(p:%, q:%, cut:I):% == {
			import from UnivariatePolynomialKaratsuba(R);
			assert(~zero? p); assert(~zero? q);
			assert(~one? p); assert(~one? q);
			assert(cut > 0);
			dp := degr p;
			dq := degr q;
			result:Coeffs := new(next(d := dp + dq), 0);
			cp := coeffs p;
			cq := coeffs q;
			m := max(dp, dq);
			if m < cut then times!(result, cp, dp, cq, dq);
			else karatsuba!(result, cp, dp, cq, dq, cut, times!);
			intdom? => poly(next d, d, result);
			(dg := computeDegree(d, result)) < 0 => 0;
			poly(next d, dg, result);
		}

		-- computes the terms x^l to x^h of a*b using naive mult
		-- returns the result as a poly of degree at most h-l
		local times1(a:%, b:%, l:Z, h:Z):% == {
			TRACE("dup::times1:a = ", a);
			TRACE("dup::times1:b = ", b);
			TRACE("dup::times1:l = ", l);
			TRACE("dup::times1:h = ", h);
			zero? a or zero? b => 0;
			da := degr a;
			db := degr b;
			H := min(da + db, machine h);
			TRACE("dup::times1:H = ", H);
			L := machine l;
			TRACE("dup::times1:L = ", L);
			H < L => 0;
			Ca := coeffs a;
			Cb := coeffs b;
			r:Coeffs := new(sz := next(H - L), 0);
			for i in 0..min(da, H) repeat {
				TRACE("dup::times1:i = ", i);
				if ~zero?(ca := Ca.i) then {
					k := max(0, i-L);
					for j in max(0,L-i)..min(db,H-i) repeat{
						TRACE("dup::times1:k = ", k);
						TRACE("dup::times1:j = ", j);
						if ~zero?(cb := Cb.j) then
							r.k := r.k + ca * cb;
						k := next k;
					}
				}
			}
			(d := computeDegree(H - L, r)) < 0 => 0;
			TRACE("dup::times1:d = ", d);
			poly(sz, d, r);
		}
	}

	if R has IntegralDomain then {
		-- computes the low-part of pp / q, destroying p, but not q.
		-- pp is considered to have degree d
		-- p is considered to be low-part of a polynomial pp which is
		-- divisible by q.
		-- trailingDegree(q) 0-coefficients of pp are not included in p.
		local quotlow!(p:%, q:%, d:Z):% == {
			zero? p => 0;
			(tcoeff, tdeg) := trailingTerm q;
			div := quotientBy tcoeff;
			-- one? p => term(div 1, 0); TM - REMOVED 9/11/99
			zero?(degree q - tdeg) => {
				r:% := 0;
				for term in p repeat {
					(a, n) := term;
					r := add!(r, div a, n);
				}
				r;
			}
			dr1 := next d;
			r:% := monomial dr1;
			(a, n) := trailingTerm p;
			while ~zero?(a) repeat {
				c := div a;
				r := add!(r, c, n);
				-- third parameter off add! can be negative !!!!
				p := add!(p, -c, n - tdeg, q, n, d);
				(a, n) := trailingTerm p;
			}
			r - monomial dr1;
		}

		-- p is considered to be the high-part of a polynomial pp which
		-- is divisible by q.
		-- computes the high-part of pp / q, destroying p, but not q.
		local quothigh!(p:%, q:%):% == {
			zero? p => 0;
			tdeg := trailingDegree q;
			lcoeff := leadingCoefficient q;
			div := quotientBy lcoeff;
			one? p => term(div 1, 0);
			r:% := 0;
			zero?(degree q - tdeg) => {
				for trm in p repeat {
					(a, n) := trm;
					r := add!(r, div a, n::Z);
				}
				r;
			}
			dq := degree q;
			dp := degree p;
			while dp >= 0 repeat {
				c := div(leadingCoefficient p);
				r := add!(r, c, dp);
				-- third parameter of add! can be negative !!!!
				h := dp - dq;
				p := add!(p, -c, h, q, max(0, tdeg + h), dp);
				dp := degree p;
			}
			r;
		}
	}

	if R has CommutativeRing then {
		-- lp, hp, lq, hq and s arbitrary, uses naive multiplication
		local smalltimes(p:%, lp:Z, hp:Z, q:%, lq:Z, hq:Z, s:Z):% == {
			TRACE("dup::smalltimes:p = ", p);
			TRACE("dup::smalltimes:lp = ", lp);
			TRACE("dup::smalltimes:hp = ", hp);
			TRACE("dup::smalltimes:q = ", q);
			TRACE("dup::smalltimes:lq = ", lq);
			TRACE("dup::smalltimes:hq = ", hq);
			TRACE("dup::smalltimes:s = ", s);
			zero? p => 0;
			zero? q => 0;
			s < 0 => 0;
			Hpi := min(degr p, machine hp);
			Hqi := min(degr q, machine hq);
			lpi := max(machine lp, 0);
			lqi := max(machine lq, 0);
			Hpi < lpi or Hqi < lqi => 0;
			Cp := coeffs p;
			Cq := coeffs q;
			si := machine s;
			s1 := next si;
			c:Coeffs := new(s1, 0);
			TRACE("dup::smalltimes:lpi = ", lpi);
			TRACE("dup::smalltimes:si = ", si);
			TRACE("dup::smalltimes:Hpi = ", Hpi);
			for i in lpi..min(Hpi, lpi + si) repeat {
				TRACE("dup::smalltimes:i = ", i);
				if ~zero?(cp := Cp.i) then {
					TRACE("dup::smalltimes:cp = ",cp);
					k := i - lpi;
					TRACE("dup::smalltimes:k = ", k);
					for j in lqi..min(Hqi,lqi+si-k) repeat {
						TRACE("dup::smalltimes:j = ",j);
						TRACE("dup::smalltimes:k = ",k);
						if ~zero?(cq := Cq.j) then {
							TRACE("dup::smalltimes:cq = ",cq);
							TRACE("dup::smalltimes:c.k = ",c.k);
							c.k := add!(c.k, cp*cq);
							TRACE("dup::smalltimes:c.k = ",c.k);
						}
						k := next k;
					}
				}
			}
			(d := computeDegree(si, c)) < 0 => 0;
			poly(s1, d, c);
		}

		-- dir in {-1, 1}, l = max(0, L).
		-- returns p_l+p_(l+1)*x+...+p_h*x^(h-l) if dir = 1 and
		-- p_h+p_(h-1)*x+...+p_l*x*(h-l) if dir = -1.
		local subpoly(p:%, L:Z, h:Z, dir:Z):% == {
			TRACE("dup::subpoly:p = ", p);
			TRACE("dup::subpoly:L = ", L);
			TRACE("dup::subpoly:h = ", h);
			TRACE("dup::subpoly:dir = ", dir);
			zero? p => 0;
			l := max(0, L);
			dp := degr p;
			newh := min(machine h, dp);
			li := machine l;
			newh < li => 0;
			if one? dir then {
				d := newh - li;
				c:Coeffs := new1 d;
				cp := coeffs p;
				k := li;
				for i in 0..d repeat {
					c.i := cp.k;
					k := next k;
				}
			}
			else {
				d := machine(h - l);
				c:Coeffs := new(next d, 0);
				cp := coeffs p;
				k := newh;
				for i in (machine(h) - newh)..d repeat {
					c.i := cp.k;
					k := prev k;
				}
			}
			(deg := computeDegree(d, c)) < 0 => 0;
			poly(next d, deg, c);
		}

		-- compute p*q up to x^s included, use Kara if needed
		local times(p:%, q:%, s:Z):% == {
			TRACE("dup::times:p = ", p);
			TRACE("dup::times:q = ", q);
			TRACE("dup::times:s = ", s);
			zero? p or zero? q => 0;
			dp := degree p;
			dq := degree q;
			h := min(dp + dq, s);
			zero? dq => smalltimes(p, 0, h, q, 0, 0, h); 
			u := (10 * (h - dq)) quo dq;
			u >= 30 => truncate!(p * q, next h);
			u < 3 => truncate!(halftimes(p,0,h,q,0,h,h), next h);
			e := 10 - u;
			if u >= 13 then e := 20 - u;
			m := h - dq + ((e * dq) quo 10);
			m1 := next m;
			r := copy q;
			r := subpoly(p, 0, m, 1) * r;
			truncate!(add!(r, 1, m1,
				halftimes(p, m1, h, q, 0, h-m1, h-m1), 0, h),
				next h);
		}

		macro {
			KARAPC == 70;
			KARACOEFF == 3;
		}

		-- b:Z == 70;
		-- t:Z == 3 * karaCutoff::Z;
		-- hp - lp = hq - lq = s
		local halftimes(p:%, lp:Z, hp:Z, q:%, lq:Z, hq:Z, s:Z):% == {
			import from R;
			assert(s = hp - lp); assert(s = hq - lq);
			s < KARACOEFF * karaCutoff::Z =>
					smalltimes(p, lp, hp, q, lq, hq, s);
			m := (KARAPC * s) quo 100;
			m1 := next m;
			r := subpoly(p, lp, lp+m, 1) * subpoly(q, lq, lq+m, 1);
			r := add!(r, 1, m1,
				halftimes(p, lp+m1, hp, q,lq,lq+s-m1,s-m1),0,s);
			add!(r, 1, m1,
				halftimes(q, lq+m1, hq, p,lp,lp+s-m1,s-m1),0,s);
		}
	}

	if R has IntegralDomain then {
		local divDiffProdOrdinary(a1:%, a2:%, b1:%, b2:%, q:%):% == {
			import from Z;
			TRACE("dup::ddprodOrd:a1 = ", a1);
			TRACE("dup::ddprodOrd:a2 = ", a2);
			TRACE("dup::ddprodOrd:b1 = ", b1);
			TRACE("dup::ddprodOrd:b2 = ", b2);
			TRACE("dup::ddprodOrd:q = ", q);
			degnumer := max(degree a1 + degree a2,
						degree b1 + degree b2);
			degdenom := degree q;
			degres := degnumer - degdenom;
			termshigh := (next degres) quo 2;
			termslow := (next degres) - termshigh;
			tdeg := trailingDegree q;
			highnumer1 := times1(a1, a2,
					next(degnumer-termshigh), degnumer);
			TRACE("dup::ddprodOrd:high1 = ", highnumer1);
			highnumer2 := times1(b1, b2,
					next(degnumer - termshigh), degnumer);
			TRACE("dup::ddprodOrd:high2 = ", highnumer2);
			highnumer1 := highnumer1 - highnumer2;
			lownumer1 := times1(a1, a2, tdeg, prev(tdeg+termslow));
			TRACE("dup::ddprodOrd:low1 = ", lownumer1);
			lownumer2 := times1(b1, b2, tdeg, prev(tdeg+termslow));
			TRACE("dup::ddprodOrd:low2 = ", lownumer2);
			lownumer1 := lownumer1 - lownumer2;
			lowres := quotlow!(lownumer1, q, prev termslow);
			TRACE("dup::ddprodOrd:lowres = ", lowres);
			highres := quothigh!(highnumer1, q);
			TRACE("dup::ddprodOrd:highres = ", highres);
			add!(lowres, 1, termslow::Z, highres, 0, degres);
		}

		local divDiffProdKaratsuba(a1:%, a2:%, b1:%, b2:%, q:%):% == {
			import from Z, R;
			TRACE("dup::ddprodKara:a1 = ", a1);
			TRACE("dup::ddprodKara:a2 = ", a2);
			TRACE("dup::ddprodKara:b1 = ", b1);
			TRACE("dup::ddprodKara:b2 = ", b2);
			TRACE("dup::ddprodKara:q = ", q);
			da1 := degree a1;
			da2 := degree a2;
			db1 := degree b1;
			db2 := degree b2;
			degnumer := max(da1 + da2, db1 + db2);
			degdenom := degree q;
			degres := degnumer - degdenom;
			tdeg := trailingDegree q;
			termshigh := (next degres + tdeg) quo 2;
			termslow := (next degres + tdeg) - termshigh;
			lowb := {
				termslow <= tdeg => -1@Z;
				prev termslow;
			}
			TRACE("dup::ddprodKara:lowb = ", lowb);
			highb := next(degnumer - termshigh);
			TRACE("dup::ddprodKara:highb = ", highb);
			(al, ah) := lowHighPart(a1, a2, lowb, highb);
			TRACE("dup::ddprodKara:al = ", al);
			TRACE("dup::ddprodKara:ah = ", ah);
			(bl, bh) := lowHighPart(b1, b2, lowb, highb);
			TRACE("dup::ddprodKara:bl = ", bl);
			TRACE("dup::ddprodKara:bh = ", bh);
			lownumer := al - bl;
			highnumer := ah - bh;
			lownumer := shiftright!(lownumer, machine tdeg);
			lowres := quotlow!(lownumer, q, prev termslow - tdeg);
			TRACE("dup::ddprodKara:lowres = ", lowres);
			highres := quothigh!(highnumer, q);
			TRACE("dup::ddprodKara:highres = ", highres);
			add!(lowres, 1, max(termslow-tdeg,0),highres,0,degres);
		}

		
		divDiffProd(a1:%, a2:%, b1:%, b2:%, q:%):% == {
			karaCutoff < 0 =>
				divDiffProdOrdinary(a1, a2, b1, b2, q);
			divDiffProdKaratsuba(a1, a2, b1, b2, q);
		}

		local divSumProdOrdinary(a1:%,a2:%,b1:%,b2:%,c1:%,c2:%,q:%):%=={
			import from Z;
			degnumer := max(degree a1 + degree a2,
					max(degree b1 + degree b2,
						degree c1 + degree c2));
			degdenom := degree q;
			degres := degnumer - degdenom;
			termshigh := (next degres) quo 2;
			termslow := (next degres) - termshigh;
			tdeg := trailingDegree q;
			highnumer1 := times1(a1, a2,
					next(degnumer - termshigh), degnumer);
			highnumer2 := times1(b1, b2,
					next(degnumer - termshigh), degnumer);
			highnumer3 := times1(c1, c2,
					next(degnumer - termshigh), degnumer);
			highnumer1 := highnumer1 + highnumer2 + highnumer3;
			lownumer1 := times1(a1, a2, tdeg, prev(tdeg+termslow));
			lownumer2 := times1(b1, b2, tdeg, prev(tdeg+termslow));
			lownumer3 := times1(c1, c2, tdeg, prev(tdeg+termslow));
			lownumer1 := lownumer1 + lownumer2 + lownumer3;
			lowres := quotlow!(lownumer1, q, prev termslow);
			highres := quothigh!(highnumer1, q);
			add!(lowres, 1, termslow::Z, highres, 0, degres);
		}

		local divSumProdKaratsuba(a1:%,a2:%,b1:%,b2:%,c1:%,c2:%,q:%):%==
		{
			import from Z;
			da1 := degree a1;
			da2 := degree a2;
			db1 := degree b1;
			db2 := degree b2;
			dc1 := degree c1;
			dc2 := degree c2;
			degnumer := max(max(da1 + da2, db1 + db2), dc1 + dc2);
			degdenom := degree q;
			degres := degnumer - degdenom;
			tdeg := trailingDegree q;
			termshigh := (next degres + tdeg) quo 2;
			termslow := (next degres + tdeg) - termshigh;
			lowb := {
				termslow <= tdeg => -1@Z;
				prev termslow;
			}
			highb := next(degnumer - termshigh);
			(al, ah) := lowHighPart(a1, a2, lowb, highb);
			(bl, bh) := lowHighPart(b1, b2, lowb, highb);
			(cl, ch) := lowHighPart(c1, c2, lowb, highb);
			lownumer := al + bl + cl;
			highnumer := ah + bh + ch;
			lownumer := shiftright!(lownumer, machine tdeg);
			lowres := quotlow!(lownumer, q, prev termslow - tdeg);
			highres := quothigh!(highnumer, q);
			add!(lowres, 1, max(termslow-tdeg,0),highres,0,degres);
		}

		divSumProd(a1:%, a2:%, b1:%, b2:%, c1:%, c2:%, q:%):% == {
			karaCutoff < 0 =>
				divSumProdOrdinary(a1, a2, b1, b2, c1, c2, q);
			divSumProdKaratsuba(a1, a2, b1, b2, c1, c2, q);
		}

		local lowHighPart(a:%, b:%, l:Z, h:Z):(%, %) == {
			TRACE("dup::lowHighPart: a = ", a);
			TRACE("dup::lowHighPart: b = ", b);
			TRACE("dup::lowHighPart: l = ", l);
			TRACE("dup::lowHighPart: h = ", h);
			da := degree a;
			db := degree b;
			if da < db then
				(a, da, b, db) := (b, db, a, da);
			if h > da + db then {
				TRACE("dup::lowHighPart: h = ", h);
				high:% := 0;
				TRACE("dup::lowHighPart: high = ", high);
				low:% := { l < 0 => 0; times(a, b, l) }
				TRACE("dup::lowHighPart: low = ", high);
			}
			else {
				if l < 0 then {
					low:% := 0;
					d := da + db - h;
					ra := subpoly(a, da - d, da, -1);
					rb := subpoly(b, db - d, db, -1);
					high := times(ra, rb, d);
					high := subpoly(high, 0, d, -1);
				}
				else {
					if (h - min(da + db - h, db) <= l)
						or (l + min(l, db) >= h) then {
						s := a * b;
						low := subpoly(s, 0, l, 1);
						high := subpoly(s, h, da+db, 1);
					}
					else {
						low := times(a, b, l);
					TRACE("dup::lowHighPart: low = ", low);
						d := da + db - h;
					TRACE("dup::lowHighPart: d = ", d);
						ra := subpoly(a, da-d, da, -1);
					TRACE("dup::lowHighPart: ra = ", ra);
						rb := subpoly(b, db-d, db, -1);
					TRACE("dup::lowHighPart: rb = ", rb);
						high := times(ra, rb, d);
					TRACE("dup::lowHighPart: high = ",high);
						high := subpoly(high, 0, d, -1);
					TRACE("dup::lowHighPart: high = ",high);
					}
				}
			}
			(low, high);
		}
	}

	trailingTerm(p:%):(R, Z) == {
		zero? p => (0, -1);
		cp := coeffs p;
		for i in 0..prev degr p repeat
			~zero?(cp.i) => return(cp.i, i::Z);
		(cp(degr p), degree p);
	}

	local newDegree!(p:%, olddeg:I, newdeg:I, c:Coeffs):% == {
		d := {
			olddeg = newdeg => {
				zero? c newdeg => computeDegree(newdeg, c);
				newdeg;
			}
			max(olddeg, newdeg);
		}
		d < 0 => 0;
		setDegree!(p, d);
		p;
	}

	add!(p:%, q:%):% == {
		zero? q => p; zero? p => copy q;
		one? p => q + 1;
		d := min(n := degr p, m := degr q);
		m >= size p => p + q;	-- TEMPO: USE resize! LATER
		cp := coeffs p; cq := coeffs q;
		for i in 0..d repeat cp.i := cp.i + cq.i;
		for i in next d..m repeat cp.i := cq.i;
		newDegree!(p, n, m, cp);
	}

	add!(p:%, c:R, q:%):% == {
		one? c => add!(p, q);
		zero? q or zero? c => p;
		zero? p => times!(c, copy q);
		one? p => add!(times!(c, copy q), 1);
		d := min(n := degr p, m := degr q);
		m >= size p => p + c * q;	-- TEMPO: USE resize! LATER
		cp := coeffs p; cq := coeffs q;
		-- TEMPORARY: BAD OPTIMIZATION BUG (WAS 1203/1232)
		-- for i in 0..d repeat cp.i := cp.i + c * cq.i;
		i:I := 0; while i <= d repeat { cp.i := cp.i+c*cq.i; i:=next i }
		-- TEMPORARY: BAD OPTIMIZATION BUG (WAS 1203/1232)
		-- for i in next d..m repeat cp.i := c * cq.i;
		i:= next d; while i <= m repeat { cp.i := c * cq.i; i:= next i }
		newDegree!(p, n, m, cp);
	}

	add!(p:%, c:R, n:Z):% == {
		assert(n >= 0);
		TRACE("dup::add!:p = ", p);
		TRACE("dup::add!:c = ", c);
		TRACE("dup::add!:n = ", n);
		zero? c => p;
		zero? p => term(c, n);
		one? p => add!(term(c, n), 1);
		cp := coeffs p;
		nn := machine n;
		nn >= size p => add!(term(c, n), p);-- TEMPO: USE resize!
		m := degr p;
		for i in next m..prev nn repeat cp.i := 0;
		cp.nn := cp.nn + c;
		newDegree!(p, m, nn, cp);
	}

	-- Partial p = p + c x^h q
	-- For frum <= i <= upto, the i-th coefficient of p will be set to
	-- the i-th coefficient of p + c x^h q. For i < frum and i > upto
	-- the i-th coefficient of p will not change.
	-- frum <= upto, h is allowed to be negative
	add!(p:%, c:R, h:Z, q:%, frum:Z, upto:Z):% == {
		zero? q => p;
		zero? c => p;
		assert(frum <= upto);
		pp := {
			zero? p or one? p => {
				a := new1 0;
				a.0 := leadingCoefficient p;
				poly(1, 0, a);
			}
			p;
		}
		n := degr pp;
		hh := machine h;
		hm := hh + degr q;	-- hm = deg(x^h q)
		u := machine upto;
		f := machine frum;
		cp := coeffs pp; cq := coeffs q;
		lp := f;
		hp := min(n, u);
		lq := max(hh, f);
		hq := min(hm, u);
		lq > hq =>  pp;
		resize?:Boolean := hq >= size pp;
		newcp := cp;
		if resize? then {	-- TEMPORARY: use resize! later
			newcp := new1 hq;
			for i in 0..n repeat newcp.i := cp.i;
		}
		if lq > n then
			for i in next n..prev lq repeat newcp.i := 0;
		if lp > hp or lq > hp then
			for i in lq..hq repeat newcp.i := c * cq(i - hh);
		else {
			high := min(hp, hq);
			for i in max(lp, lq)..high repeat
				newcp.i := cp.i + c * cq(i - hh);
			for i in next high..hq repeat newcp.i := c * cq(i - hh);
		}
		d := computeDegree(max(hq, n), newcp);
		resize? => {
			free! cp;
			d < 0 => 0;
			poly(next hq, d, newcp);
		}
		d < 0 => 0;
		setDegree!(pp, d);
		pp
	}

	generator(p:%):Generator Cross(R, Z) == generate {
		if ~zero? p then {
			v := coeffs p;
			for i in degr p .. 0 by -1 repeat {
				a := v.i;
				if ~zero? a then yield(a, i::Z);
			}
		}
	}

	terms(p:%):Generator Cross(R, Z) == generate {
		if ~zero? p then {
			v := coeffs p;
			for i in 0 .. degr p repeat {
				a := v.i;
				if ~zero? a then yield(a, i::Z);
			}
		}
	}

	if R has FiniteCharacteristic then {
		pthPower(p:%):% == {
			zero? p => 0;
			dd := charTimes(d := degr p);
			a := new1 dd;
			for i in 0..dd repeat a.i := 0;
			c := coeffs p;
			for i in 0..d repeat a(charTimes i) := pthPower(c.i)$R;
			poly(next dd, dd, a);
		}
	}

	copy(p:%):% == {
		zero? p or one? p => p;
		a := new1(d := degr p);
		c := coeffs p;
		for i in 0..d repeat a.i := c.i;
		poly(next d, d, a);
	}

	copy!(p:%, q:%):% == {
		zero? p or one? p => copy q;
		cp := coeffs p;
		zero? q => {
			cp.0 := 0;
			setDegree!(p, 0);
			p;
		}
		copy!(p, degr q, coeffs q);
	}

	local copy!(p:%, d:I, c:Coeffs):% == {
		zero? p or one? p => {
			cp := new1 d;
			for i in 0..d repeat cp.i := c.i;
			poly(next d, d, cp);
		}
		cp := coeffs p;
		d >= size p => poly(next d, d, c); -- TEMPO: USE resize! LATER
		for i in 0..d repeat cp.i := c.i;
		setDegree!(p, d);
		p;
	}

	coefficient(p:%, n:Z):R == {
		assert(n >= 0);
		(m := machine n) > degr p => 0;
		coeffs(p).m;
	}

	setCoefficient!(p:%, n:Z, c:R):% == {
		assert(n >= 0);
		zero? p => term(c, n);
		m := machine n;
		d := degr p; cp := coeffs p;
		zero? c => {
			m > d => p;
			one? p => { assert(zero? m); 0 }
			cp.m := c;
			(deg := computeDegree(prev m, cp)) < 0 => 0;
			setDegree!(p, deg);
			p;
		}
		one? p => {
			cxn := term(c, n);
			zero? n => cxn;
			add!(cxn, p);
		}
		m >= size p => add!(term(c, n), p);	-- TEMPO: USE resize!
		cp.m := c;		-- guaranteed nonzero
		if m > d then setDegree!(p, m);
		p;
	}

	monomial!(p:%, c:R, n:Z):% == {
		assert(n >= 0);
		zero? p or one? p => term(c, n);
		if zero? c then n := 0;
		m := machine n;
		m >= size p => term(c, n);	-- TEMPO: USE resize! LATER
		cp := coeffs p;
		for i in 1..m repeat cp.i := 0;
		cp.m := c;
		setDegree!(p, m);
		p;
	}

	term(c:R, n:Z):% == {
		assert(n >= 0);
		if zero? c then n := 0;
		a := new1(m := machine n);
		for i in 0..prev m repeat a.i := 0;
		a.m := c;
		poly(next m, m, a);
	}

	minus!(p:%):% == {
		zero? p => 0;
		one? p => -1;
		c := coeffs p;
		for i in 0..degr p repeat c.i := - c.i;
		p;
	}

	-(p:%):% == {
		a := new1(d := degr p);
		c := coeffs p;
		for i in 0..d repeat a.i := - c.i;
		poly(next d, d, a);
	}
		
	reductum(p:%):% == {
		zero?(d := degr p) => 0;
		c := coeffs p;
                (deg := computeDegree(d - 1, c)) < 0 => 0;
                poly(size p, deg, c);
	}

	term?(p:%):Boolean == {
		c := coeffs p;
		zero?(c.0) and
			(zero?(d := degr p) or
				(computeDegree(prev d, c)) < 0);
	}

	(x:%) = (y:%):Boolean == {
		degr x ~= (d := degr y) => false;
		cx := coeffs x; cy := coeffs y;
		for i in 0..d repeat cx.i ~= cy.i => return false;
		true;
	}

	local times!(c:R, a:Coeffs, d:I, b:Coeffs):Coeffs == {
		for i in 0..d repeat b.i := c * a.i;
		b;
	}

	-- n = upper bound on the actual degree
	-- returns -1 if poly is in fact 0
	local computeDegree(n:I, c:Coeffs):I == {
		assert(n >= 0);
		for i in n..0 by -1 repeat ~zero?(c.i) => return i;
		-1;
	}

	(c:R) * (p:%):% == {
		zero? c => 0; one? c => p;
		d := degr p;
		cf := times!(c, coeffs p, d, new1 d);
		intdom? => poly(next d, d, cf);
		(deg := computeDegree(d, cf)) < 0 => 0;
		poly(next d, deg, cf);
	}

	times!(c:R, p:%):% == {
		zero? c or zero? p => 0;
		one? c => p; one? p => c::%;
		a := coeffs p;
		a := times!(c, a, d := degr p, a);
		intdom? => p;
		(deg := computeDegree(d, a)) < 0 => 0;
		setDegree!(p, deg);
		p;
	}

	local addsub(p:%, q:%, plus?:Boolean):% == {
		zero? q => p;
		zero? p => {plus? => q; -q }
		M := max(dp := degr p, dq := degr q);
		m := min(dp, dq);
		c := new1 M;
		cp := coeffs p; cq := coeffs q;
		if plus? then for i in 0..m repeat c.i := cp.i + cq.i;
			else  for i in 0..m repeat c.i := cp.i - cq.i;
		dp > dq => {
			for i in next m..M repeat c.i := cp.i;
			poly(next M, M, c);
		}
		dp < dq => {
			if plus? then for i in next m..M repeat c.i :=   cq.i;
				else  for i in next m..M repeat c.i := - cq.i;
			poly(next M, M, c);
		}
		(d := computeDegree(M, c)) < 0 => 0;
		poly(next M, d, c);
	}

	equal?(a:%, b:%, c:%, n:Z):Boolean == {
		n <= 0 or n > (max$I)::Z => true;
		zero? b or zero? c => {
			(coeff, deg) := trailingTerm a;
			zero? a or deg >= n;
		}
		m := machine n;
		db := degr b; cb := coeffs b;
		dc := degr c; cc := coeffs c;
		M := db + dc;
		for i in 0..min(m, M) repeat {
			z:R := 0;	-- will be coeff(b c, x^i)
			k := min(i, dc);
			for j in i-k .. min(i, db) repeat {
				z := z + cb.j * cc.k;
				k := prev k;
			}
			z ~= coefficient(a, i::Z) => return false;
		}
		for i in next M..m repeat
			~zero? coefficient(a, i::Z) => return false;
		true;
	}

	local naiveProduct(p:%, q:%):% == {
		assert(~zero? p); assert(~zero? q);
		assert(~one? p); assert(~one? q);
		dp := degr p;
		dq := degr q;
		M := dp + dq;
		c:Coeffs := new(next M, 0);	-- init all to 0
		times!(c, coeffs p, dp, coeffs q, dq);
		intdom? => poly(next M, M, c);
		(d := computeDegree(M, c)) < 0 => 0;
		poly(next M, d, c);
	}

	-- c = c + cp cq
	local times!(c:Coeffs, cp:Coeffs, dp:I, cq:Coeffs, dq:I):() ==
		times!(c, 0, cp, 0, dp, cq, 0, dq);

	-- nc, np and nq are offsets in c, p and q respectively
	local times!(c:Coeffs, nc:I, cp:Coeffs, np:I, dp:I,
					cq:Coeffs, nq:I, dq:I):() == {
		for i in 0..dp repeat {
			k := i;
			zp := cp(i+np);		-- coeff(p, x^{i-1})
			if ~zero?(zp) then {
				for j in 0..dq repeat {
					if ~zero?(zq := cq(j+nq)) then
						c(k+nc) := c(k+nc) + zp * zq;
					k := next k;
				}
			}
		}
	}

	if R has SerializableType then {
		(port:BinaryWriter) << (a:%):BinaryWriter == {
			import from I;
			port := port << (n := next degr a);
			write(port, coeffs a, n);
		}

		<< (port:BinaryReader):% == {
			n:I := << port;
			zero? n => 0;
			assert(n > 0);
			poly(n, prev n, read(port, n));
		}
	}
}

#if ALDORTEST
---------------------- test dup.as --------------------------
#include "algebra"
#include "aldortest"

macro {
        Z == Integer;
        Zx == DenseUnivariatePolynomial Z;
	Zxt == DenseUnivariatePolynomial Zx;
}

degree():Boolean == {
	import from Z, Zx;
	x := monom;
	p := (x - 1) * (x + 1);
	degree p = 2 and leadingCoefficient p = 1 and zero? p(-1@Z);
}

exactQuotient():Boolean == {
        import from Zx, Partial Zx;

        x := monom;
	a := x - 1;
	b := x + 1;
	p := a * b;
        q := exactQuotient(p, a);	-- must be b
        f := exactQuotient(p, x);	-- must be failed
        ~(failed? q) and failed? f and retract(q) = b;
}

diff():Boolean == {
	import from Z, Zx, Zxt;
	x:Zx := monom;
	t:Zxt := monom;
	p := x * t + (x^2)::Zxt;
	D:Derivation(Zxt) := lift(derivation, t);    -- t' = t
	q := D p;                       -- must be (1 + x) t + 2 x
	r := q - p;                     -- must be t + 2 x - x^2
	m := x * (x - 2::Zx);
	degree r = 1 and leadingCoefficient r = 1
		and zero?(reductum(r) + m::Zxt);
}

hgcd(a:Zx, b:Zx):Zx == {
	import from Partial Zx, HeuristicGcd(Z, Zx);
	(g, a, b) := heuristicGcd(a, b);
	retract g;
}

mgcd(a:Zx, b:Zx):Zx == {
	import from Partial Zx, ModularUnivariateGcd(Z, Zx);
	(g, a, b) := modularGcd(a, b);
	failed? g => error "mgcd: modularGcd failed";
	retract g;
}

heugcd():Boolean == gcd hgcd;

modgcd():Boolean == gcd mgcd;

gcd(ggt:(Zx,Zx) -> Zx):Boolean == {
	import from Z, Zx;

	x := monom;
	p := x^8 + x^6 - 3*x^4 - 3*x^3 + 8*x^2 +2*x - 5@Z ::Zx;
	q := 3*x^6 + 5*x^4 -4*x^2 -9*x + 21@Z ::Zx;
	r := x^2 + 1;
	g := ggt(p, q);
	rg := ggt(r * p, r * q);
	g = 1 and rg = r;
}

stdout << "Testing sit__dup..." << endnl;
aldorTest("degree", degree);
aldorTest("exactQuotient", exactQuotient);
aldorTest("diff", diff);
--aldorTest("heugcd", heugcd);
--aldorTest("modgcd", modgcd);
--TESTS FAILED: do not terminate
stdout << endnl;
#endif

