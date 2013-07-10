---------------------------- alg_sup1.as ----------------------------------
-- Copyright (c) Marc Moreno Maza 2002
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) INRIA (France), USTL (France), UWO (Ontario) 2002
-----------------------------------------------------------------------------

#include "algebra"


#if ALDOC
\thistype{SparseUnivariatePolynomial1}
\History{Marc Moreno Maza}{13/05/2002}{created}
\History{Marc Moreno Maza}{21/06/2002}{last update}
\Usage{ import from \this~R\\ import from \this(R, x) }
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em x} & \altype{Symbol} & The variable name (optional)\\
}
\Descr{\this(R, x) implements sparse univariate polynomials with coefficients
in R.}
\begin{exports}
\category{\altype{UnivariatePolynomialRing} R}\\
\end{exports}
#endif

macro {
	Z == Integer;
	NNI == Integer;
	}

SparseUnivariatePolynomial1(R:Join(ArithmeticType, ExpressionType),
	avar:Symbol == new()): UnivariatePolynomialRing(R) == SparseUnivariatePolynomial0(R,avar) add {


	------------------------
	--% Rep and imports  %--
	------------------------

	Term == Record(co: R, ex: NNI);
	Rep	== List Term;
		-- always ordered terms, decreasing wrt exponent's order
		-- and non-zero coefficients
	import from Term, Rep, R, Z;

	-----------------------
	--% local constanta %--
	-----------------------
	local commutativeRing?:Boolean		== R has CommutativeRing;
	local integralDomain?:Boolean		== R has IntegralDomain;
	local orderedArithmeticType?:Boolean	== R has OrderedArithmeticType;
	local characteristicZero?:Boolean	== R has CharacteristicZero;
        local goodRing?:Boolean == integralDomain? and characteristicZero?;

	local numberSystem?:Boolean ==
		(R has PrimeFieldCategory0) or (R has IntegerCategory);

	------------------------
	--% local functions  %--
	------------------------
	local equalTerm?(tx: Term, ty: Term): Boolean == {
		tx.ex = ty.ex and tx.co = ty.co;
	}
        local -(t:Term):Term	== [-t.co, t.ex]$Term;
	local neg!(tt: Term): Term  == {
		tt.co := -tt.co;
		tt;
	}
	local copyterm(t: Term): Term == [explode t];
	local deepCopy(l:List Term):List Term == {
		cl: List Term := empty;
		for t in l repeat {
			cl := cons([t.co, t.ex], cl);
		}
		reverse! cl;
	}
	local tepo(tco: R, tex: NNI, rx: Rep): Rep == {
		if integralDomain? then {
			termTimes(tx:Term):Term == [tco*tx.co, tex+tx.ex];
			map(termTimes)(rx);
		}
		else {
			[[r, tex + tx.ex] for tx in rx |
				not zero? (r := tco * tx.co)];
		}
	}
	--- constant * pol + term * pol
	--- assumes that constant and term are non-zero
	local pomopo!(isone?:Boolean,c:R,xx:Rep,tco:R,tex:NNI,yy:Rep): Rep == {
                -- assert(not zero? tco);
		local res, newend, last: Rep;
		res := empty;
		repeat {
			empty? xx => break;
			empty? yy => break;
			tx := first xx;
			ty := first yy;
			exy := tex + ty.ex;
			if tx.ex > exy then {
				if not isone? then tx.co := c * tx.co;
				if (not(isone?) and not(integralDomain?)
				    and (zero? tx.co)) then {
					xx := rest xx;
					iterate;
				}
				newend := xx;
				xx := rest xx;
			}
			else {
				coy := tco * ty.co;
				yy := rest yy;
				if exy > tx.ex then {
					if (not(integralDomain?) and
					    (zero? coy)) then {
						iterate;
					}
					newend := [[coy, exy]];
				}
				else {
					if not isone? then tx.co:= c*tx.co;
					tx.co := tx.co + coy;
					if zero? tx.co then {
						xx := rest xx;
						iterate;
					}
					else {
						newend := xx;
						xx := rest xx;
					}
				}
			}
			if empty? res then {
				res := newend;
				last := res;
			}
			else {
				-- WAS  last.rest := newend;
				setRest!(last,newend); 
				last := newend;
			}
		}
		if ~empty?(xx) then { -- then end is c * xx
			if isone? then newend := xx;
			else newend := rep times!(c, per xx);
		}
		else {  -- then end is (tco, tex) * yy
			newend := tepo(tco, tex, yy);
		}
		empty? res => newend;
		-- WAS  last.rest := newend;
		setRest!(last,newend); 
		res;
	}
        local caddmp!(c1: R, x: %, c2: R, e: NNI, y: %): % == {
		(zero? c2) or (zero? y) => times!(c1, x);
		(zero? c1) or (zero? x) => per tepo(c2, e, rep y);
		ground? x => add!(per tepo(c2, e, rep y), c1 * leadingCoefficient(x), 0@Z);
		per pomopo!(one? c1, c1, rep x, c2, e, rep y);
	}
	if R has CommutativeRing then {
	local binomialTheorem__4(xco: R, xex: NNI, yy: Rep, n: NNI): % == {
		TRACE("sup1::binomialTheorem__4: xco = ", xco);
		TRACE("sup1::binomialTheorem__4: xex = ", xex);
		TRACE("sup1::binomialTheorem__4: n = ", n);
                -- assert(characteristicZero?);
		lt: List Term := [[xco, xex]];
		local co: R; 
		local ex: NNI;
		for i in 2..n repeat {
			(co, ex) := explode first lt;
			lt := cons( [xco * co, xex + ex], lt);
		}
		yn: % := per yy;
		res: Rep := [first lt];
		i: Integer := 1;
		ni: Integer := n:: Integer;
		bincoef: Integer := ni;
		for tx in rest lt repeat {
			(co, ex):= explode tx;
                        res:= pomopo!(true,1$R,res,bincoef*co,ex,rep yn);
			bincoef := ((ni -i) * bincoef) quo (i + 1);
			i := i + 1;
			yn := yn * (per yy);
		}
		per(res) + yn;
	}
	}
	local gen(l:List Term):Generator Cross(R, Z) == generate {
		import from Boolean;
		while ~empty? l repeat {
			t := first l;
			l := rest l;
			yield(t.co, t.ex);
		}
	}
	local makeCanonical!(xx: Rep): Rep == {
		repeat {
			empty? xx => break;
			tx := first xx;
			zero? tx.co => xx := rest xx;
			break;
		}
		empty? xx => xx;
		local current, next: Rep;
		current := xx;
		next := rest xx;
		repeat {
			repeat {
				empty? next => break;
				tx := first next;
				zero? tx.co => next := rest next;
				break;
			}
			setRest!(current,next);
			empty? next => break;
			current := next;
			next := rest next;
		}
		xx;
	}
	-- returns all the terms of p of degree stricly less than n
	local trunc(p:%, n:Z):% == {
		while (~zero? p) and degree p >= n repeat p := reductum p;
		p;
	}



------add!(x:%, d:R, h:Z, y:%, n:Z, m:Z)--------modified nov,2004


	add!(x:%, d:R, h:Z, y:%, n:Z, m:Z):% == {
		zero? y => x;
		zero? d => x;
		assert(n <= m);
		yy:Rep:= rep(copy(y));
                yl:=n-h;
                yh:=m-h;
		repeat {
			empty? yy => break;
			t := first yy;
			t.ex > yh => yy := rest yy;
			break;	}
		empty? yy => x;
                yl<=0 =>caddmp!(1,x,d,h, per yy);
		yyy: Rep := yy;
		t := first yyy;
                t.ex < yl => x;
		zzz: Rep := rest yyy;
		repeat {
                        empty? zzz => break;
			t := first zzz;
			t.ex < yl => break;
			yyy := zzz;
			zzz := rest zzz;
		}
                setRest!(yyy,empty);
		newy:=per yy;
                caddmp!(1,x,d,h,newy);
 	}




	-------------------------------------------------
	--% Exports as UnivariatePolynomialRing(R) %--
	-------------------------------------------------
	-- UnivariateFreeRing(R)
	-- if (R has Parsable) then Parsable
        -- compose: (%, %) -> %
        -- translate: (%, R) -> %

	--add!(x:%, d:R, h:Z, y:%, n:Z, m:Z):% == {
		--xx: Rep := rep caddmp!(1,x,d,h,y);
		--repeat {
			--empty? xx => break;
			--t := first xx;
			--t.ex > m => xx := rest xx;
			--break;
		--}
		--empty? xx => 0;
		--yy: Rep := xx;
		--t := first yy;
		--t.ex < n => 0;
		--zz: Rep := rest yy;
		--repeat {
			--empty? zz => break;
			--t := first zz;
			--t.ex < n => break;
			--yy := zz;
			--zz := rest zz;
		--}
		--setRest!(yy,empty);
		--per xx;
	--}




	--------------------------------------
        --% Exports as UnivariateFreeRing(R) %--
	--------------------------------------
	-- CopyableType
	-- IndexedFreeAlgebra(R, Z)
	-- UnivariateFreeLinearArithmeticType(R,Z)
        -- coerce: Vector(R) -> %
        -- vectorize!: Vector(R) -> % -> Vector(R)
        -- companion: % -> DenseMatrix(R)
        -- random: (AldorInteger, () -> R, m: AldorInteger == - 1) -> %
        -- revert: % -> %
        -- revert!: % -> %

	monomial!(x:%, r:R, e:Z):% == {
		ground? x => term(r, e);
		zero? r => 0;
		t := first rep x;
		t.ex := e; t.co := r;
		setRest!(rep x, empty);
		x;
	}

	-- if (R has GcdDomain) then 
        --        content: (%, Automorphism(R pretend Ring)) -> R
        --        primitive: (%, Automorphism(R pretend Ring)) -> (R, %)
        --        primitivePart: (%, Automorphism(R pretend Ring)) -> %

	-------------------------------
	--% Exports as CopyableType %--
	-------------------------------
        -- copy!: (%, %) -> %
	copy(p:%):% == {
		zero? p or one? p => p;
		per deepCopy rep p;
	}

	-------------------------------------------
	--% Exports as IndexedFreeAlgebra(R, Z) %--
	-------------------------------------------
	-- FreeAlgebra(R)
	-- IndexedFreeModule(R,Z)
	-- IndexedFreeLinearArithmeticType(R,Z)

	-----------------------------------------------------
	--% Exports as UnivariateFreeLinearArithmeticType(R,Z) %--
	-----------------------------------------------------
        -- apply: (TextWriter, %, Symbol) -> TextWriter !!!!!! TO DEFINE !!!!!!!!
        -- coefficients: % -> Generator(R)
        -- monom: %
        -- truncate: (%, AldorInteger) -> %
        -- truncate!: (%, AldorInteger) -> %
        -- shift: (%, AldorInteger) -> %
	local dummy:Symbol == { import from String; -"dummy"; }
	apply(p:%, x:ExpressionTree):ExpressionTree == {
			import from Boolean, R, List ExpressionTree;
			import from UnivariateMonomial(R, dummy);
			zero? p => extree(0@R);
			l:List(ExpressionTree) := empty;
			for term in rep(p) repeat {
				m := monomial(term.co, term.ex)@UnivariateMonomial(R,dummy);
				l := cons(m x, l);
			}
			assert(~empty? l);
			empty? rest l => first l;
			ExpressionTreePlus reverse! l;
	}
	shift!(a:%, n:Z):% == {
		import from Boolean;
		zero? n => a;
		l := rep a;
		assert(~empty? l);
		t := first l;
		(d := n + t.ex) < 0 => 0;
		zero? (t.ex) => term(t.co,n);
		t.ex := d;
		last := l;
		while ~empty?(l := rest l) repeat {
			t := first l;
			(d := n + t.ex) < 0 => {
				setRest!(last, empty);
				return a;
			}
			t.ex := d;
			last := l;
		}
		a;
	}

	---------------------------------
	--% Exports as FreeAlgebra(R) %--
	--------------------------------- 
	-- if (R has Ring) then Algebra(R pretend Ring)
	-- if (R has CharacteristicZero) then CharacteristicZero
	-- if (R has RittRing) then RittRing
	if R has FiniteCharacteristic then {
		local pthPowerTerm(t:Term):Term == {
			[pthPower(t.co)$R, characteristic$R * t.ex];
		}
		local pthPowerTerm!(t:Term):Term == {
			t.co := pthPower(t.co)$R;
			t.ex := characteristic$R * t.ex;
			t;
		}
		pthPower(p:%):%	 == per [pthPowerTerm t for t in rep p];
		pthPower!(p:%):% == { for t in rep p repeat pthPowerTerm! t; p };
	}

	
	commutative?: Boolean  == commutativeRing?;

	-------------------------------------------------------
	--% Exports as IndexedFreeLinearArithmeticType(R,Z) %--
	-------------------------------------------------------
	add!(x: %, r: R, e: Z, y: %): % == {
		caddmp!(1,x,r,e,y);
	}

	-------------------------------------------
	--% Exports as LinearCombinationType(R) %--
	-------------------------------------------
	add!(x:%, d:R, y:%):% == {
		-- WAS zero? d => x;
		-- WAS one? d => add!(x, y);
		-- WAS zero? x => times!(d, copy y);
		-- WAS one? x => add!(times!(d, copy y), 1);
		caddmp!(1,x,d,0,y);
	}

	---------------------------------------
	--% Exports as LinearArithmeticType %-- 
	---------------------------------------
        -- ^: (%, AldorInteger) -> %
	coerce(r: R): % == {
		zero? r => 0;
		per [[r, 0]$Term];
	} 

	---------------------------------
	--% Exports as ArithmeticType %-- 
	---------------------------------
	one? (x: %) : Boolean == {
		xx:= rep x;
                empty? xx => false;
		(empty? rest xx) and (zero? ((first xx).ex)) and (one? ((first xx).co));
	}
	1:%				== per [[1$R, 0]$Term];
	(x: %) * (r: R) : % == {
		zero? r => 0;
		if integralDomain? then {
			per [[tx.co*r, tx.ex] for tx in rep x];
		}
		else {
			per [[rr, tx.ex] for tx in rep x | 
				not zero? (rr: R := tx.co*r)];
		}

	}
	(x: %) * (y: %) : % == {
		xx := rep x;
		empty? xx => x;
		yy := rep y;
		empty? yy => y;
		zero? first(xx).ex => first(xx).co * y;
		zero? first(yy).ex => x * first(yy).co;
                local n__x: MachineInteger := #xx;
                local n__y: MachineInteger := #yy;
		if commutativeRing? and (n__x > n__y) then {
			(xx,yy) := (yy,xx);
			(x,y) := (y,x);
		}
		res : Rep := empty;
		xx := reverse xx;
		for tx in xx repeat res:=pomopo!(true,1$R,res,tx.co,tx.ex,yy);
		per res;
	}
	if R has Ring then {
	coerce(z:Integer): % == { import from R; (z::R)::%; }

	if R has CommutativeRing and R has CharacteristicZero then {
	(x: %) ^ (n: NNI): % == {
		zero? n => 1;
		n = 1  => x;
		xx := rep x;
		empty? xx => x;
		empty? rest xx => {
			r: R := (first(xx).co) ^ n;
			if zero? r then return 0;
			per [[r, n * (first xx).ex]$Term];
		}
		(xco, xex):= explode first xx;
		binomialTheorem__4(xco, xex, rest xx, n);
	}
	} else {
	(x: %) ^ (n: NNI): % == {
		import from BinaryPowering(%, Integer);
		zero? n => 1;
		n = 1  => x;
		xx := rep x;
		empty? xx => x;
		empty? rest xx => {
			r: R := (first(xx).co) ^ n;
			if zero? r then return 0;
			per [[r, n * (first xx).ex]$Term];
		}
		binaryExponentiation(x, n);
	} }
	}
}


#if ALDORTEST
---------------------- test sup1.as ---------------------------
#include "algebra"
#include "aldortest"

-- #include "algebra"
-- #include "aldorinterp"
-- #include "aldortest"

X: Symbol == -"x";
Y: Symbol == -"y";

macro {
        Z == Integer;
        Zx == SparseUnivariatePolynomial1 (Z,X);
        Zxt == SparseUnivariatePolynomial1 (Zx,Y);
}

primitiveType():Boolean == {
        import from Z, Zx;
	x := term(1,1);
	p := x^2 - 1;
	pp := x^2 - 1;
	q := x^2 + x - 1;
	(p = pp) and (p ~= q);
}

additiveType():Boolean == {
        import from Z, Zx;
	x := term(1,1);
	p: Zx := 0;
	q: Zx := x^2;
	r: Zx := 1;
	s: Zx := x;
	t: Zx := -(x^2 +x + 1);
	u := add!(p,q);
	add!(u,s);
	add!(u,r);
	minus!(u);
	(zero? p) and (q = x^2) and (u = t);
}

linearCombinationType():Boolean == {
        import from Z, Zx;
        x := term(1,1);
	p := x^2 +x + 1;
	t: Zx := - copy(p);
	times!(2,t);
	zero?(t + 2 * p);
}

freeLinearCombinationType():Boolean == {
        import from Z, Zx;
        x := term(1,1);
	f(z:Z):Z == z rem 2;
	p := 2*x^4 + x^3 + 2*x^2 + x + 2 * 1;
	fp := x^3 + x;
	q := x^4 + 2*x^3 + x^2 + 2*x + 1;
	fq := x^4 + x^2 + 1;
	r: Zx := 2*x + 4 * 1;
	fr : Zx := 0;
	s := x^4 + 2*x^3 + 4*x^2 + 2*x + 6 * 1;
	fs := x^4;
	t := 2*x^4 + 2*x^3 + 4*x^2 + 2*x + 5 * 1;
	ft: Zx := 1;
	(map(f)(p) =  fp) and (map(f)(q) =  fq) and (map(f)(r) =  fr) and (map(f)(s) =  fs) and (map(f)(t) =  ft);
}

indexedFreeLinearCombinationType():Boolean == {
        import from Z, Zx;
        x := term(1,1);
	p := 2*x^4 + 2*x^2;
	
	bool: Boolean := (coefficient(p,4) = 2) and (coefficient(p,3) = 0) and (coefficient(p,2) = 2) and (coefficient(p,1) = 0) and (coefficient(p,0) = 0);
	
	p := setCoefficient!(p,5,10);
	setCoefficient!(p,4,8);
	setCoefficient!(p,3,6);
	setCoefficient!(p,2,4);
	setCoefficient!(p,1,2);
	setCoefficient!(p,0,-1);
	q := 10*x^5 + 8*x^4 + 6*x^3 + 4*x^2 + 2*x -1;
	bool := bool and (p = q);
	p := 0;
	add!(p,2,2);
	bool := bool and (zero? p);
	p := -4*x^4 - x^2;
	p := add!(p,-5,5);
	add!(p,-3,3);
	add!(p,-1,2);
	z: Z := 0;
	add!(p,1,z);
	q := -5*x^5  -4*x^4 -3*x^3 - 2*x^2 + 1;
	bool := bool and (p = q);
}

indexedFreeModule():Boolean == {
        import from Z, Zx;
        x := term(1,1);
        p := (x - 1)*(x + 1) * x;
        (degree p = 3) and (leadingCoefficient p = 1) and (trailingDegree p = 1) and (trailingCoefficient p = -1) and (reductum p = term(-1,1));
}

univariatePolynomialAlgebra():Boolean == {
        import from Z, Zx;
        x := term(1,1);
	bool: Boolean := true;
	lZx: List Zx := [1, 0, x, -x,  x^3 + x, -x^3 + x^3 +1];
	ld: List Z := [0,1,-1,2];
	lh: List Z := [0,1,2,3];
	for p in lZx repeat {
		for d in ld repeat {
			for h in lh repeat {
				for q in lZx repeat {
					r1 := p + term(d,h) * q;
					r2 := p + times!(term(d,h),copy q);
					bool := bool and (r1 = r2);
					r3 := add!(copy p, d, h, copy q);
					bool := bool and (r1 = r3);
--					r4 := add!(copy p, d, h, copy q, 0,degree(r3));
--					bool := bool and (r1 = r4);
--					ground? r1 => iterate;
--					r3 := reductum r3;
--					r4 := add!(copy p, d, h, copy q, 0,degree(r3)-1);
--					bool := bool and (r3 = r4);
				}
			}
		}
	} 
	bool;
}

arithmeticType():Boolean == {
        import from Z, Zx;
        x := term(1,1);
	local bool: Boolean := true;
	local test: Boolean;
	lZx: List Zx := [1, 0, x, -x,  x^3 + x, -x^3 + x^3 +1, x^4+x^3+x^2+x+1];
	local q: Zx; local n0: Z; local n1: Z; 
	n0 := 0;
	n1 := 10;
	for p in lZx repeat {
		q := 1;
		for n in n0..n1 repeat {
			test := (q = p^n);
			if ~test then {
				stdout << "p ? " << p << endnl;
				stdout << "n ? " << n << endnl;
				stdout << "q ? " << q << endnl;
				stdout << "p^n ? " << p^n << endnl << endnl;
			}
			bool := bool and test;
			q := q * p;
		}
	}
	bool;
}

stdout << "Testing alg__sup1..." << endnl;
aldorTest("primitiveType",primitiveType );
aldorTest("additiveType",additiveType );
aldorTest("linearCombinationType",linearCombinationType );
aldorTest("freeLinearCombinationType",freeLinearCombinationType );
aldorTest("indexedFreeLinearCombinationType",indexedFreeLinearCombinationType );
aldorTest("indexedFreeModule",indexedFreeModule );
aldorTest("univariatePolynomialAlgebra",univariatePolynomialAlgebra);
aldorTest("arithmeticType",arithmeticType);
stdout << endnl;




#endif

