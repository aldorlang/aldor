#include "algebra"

-- Some helpful routines for working with univariate polynomials with
-- coefficients in a finite field. 

macro {
    Z   == Integer;
    MI  == MachineInteger;
    UP  == UnivariatePolynomialAlgebra0;

    REC    == Record(fact: UPf, exp: Integer);
    PA     == PrimitiveArray(F);
    M      == PrimitiveArray(PA);
}


+++ Author: Stephen Michael Wilson
+++ Last Update: 10/14/2004

ModularUnivariateTools( E   : IntegralDomain,
			UPe : UP E, 
			F   : Field, 
			UPf : UP F,
			modularImage: E -> F,
			retractImage: F -> E ): with {

    modularImage: UPe -> UPf;
    retractImage: UPf -> UPe;

    sqFreeYun: UPf -> (F, Product(UPf));

#if 0
    berlekamp: UPf -> List(UPf);
#endif

    eea: (UPf, UPf) -> (UPf, UPf, UPf);
    EEAlift: (UPe, UPe, E, Z) -> (UPe, UPe);

} == add {

    local Pz: E == (characteristic$F)::E;
    local Pn: Z == characteristic$F;
    local Pmi: MI == machine(characteristic$F)$Z;

    modularImage(f: UPe): UPf == {
	local pf: UPf := 0;
	for term in f repeat {
	    (t, e) := term;
	    pf := add!(pf, modularImage(t), e);
	}
	return pf;
    }

    retractImage(pf: UPf): UPe == {
	local f: UPe := 0;
	for term in pf repeat {
	    (t, e) := term;
	    f := add!(f, retractImage(t), e);
	}
	return f;
    }

    local sqFreeGCD(u: UPf, v: UPf): (UPf, UPf, UPf) == {
	local d: UPf := gcd(u, v);
	return (d, monicQuotient(u,d), monicQuotient(v,d));
    }

    local ldegree(l: List(REC)): Z == {
	import from REC;
	i: Z := 0;
	for e in l repeat  i := i + e.exp*degree(e.fact);
	return i;
    }

    local reduceExponents(f: UPf): UPf == {
	import from Z;
	local res: UPf := 0;
	local constant: F := 0;

	if (trailingDegree(f) = 0) then 
	    constant := trailingCoefficient(f);
	
	f := f - term(constant, 0);

	for m in f repeat {
	    (t, e) := m;
	    res := add!(res, t, quotient(e, Pn));
	}

	res := add!(res,term(constant, 0));
	return res;
    }

    sqFreeYun(u: UPf): (F, Product(UPf)) == {
	import from REC, Z;
	leadc: F := leadingCoefficient(u);
	local l: List(REC);
	local r: Product(UPf) := 1;

	u := inv(leadc)*u;
	l := sqFreeYun(u);
	for e in l repeat 
	    r := times!(r, e.fact, e.exp);
	return (leadc, r);
    }

    local sqFreeYun(u: UPf): List(REC) == {
	import from Z, REC;

	local {
	    lr: List(REC) := empty;
	    lret: List(REC) := empty;
	    lv: List(REC) := empty;
	    t: UPf; d: UPf; v: UPf; w: UPf;
	    e: Z := 1;
	}

	(t, v, w) := sqFreeGCD(u, differentiate(u));
	
	if not one?(t) then repeat {
	    d := w-differentiate(v);
	    if d = 0 then break;
	    (d, v, w) := sqFreeGCD(v, d);
	    if not one?(d) then 
		lr := append!(lr, [d, e]);
	    e := next(e);
	}

	lr := append!(lr, [v, e]);

	if one?(t) or (ldegree(lr) = degree(u)) then return lr;

	d := 1;
	for j in 1..#lr repeat
	    d := times!(d,(lr.j.fact)^(prev(lr.j.exp)));

	v := monicQuotient!(t, d);
	v := reduceExponents(v);
	
	lv := sqFreeYun(v);
	
	for j in 1..#lr repeat {
	    for k in 1..#lv repeat {
		w := gcd(lr.j.fact, lv.k.fact);
		if not one?(w) then {
		    lret := append!(lret, [w, lr.j.exp+lv.k.exp*Pn]);
		    lr.j.fact := monicQuotient!(lr.j.fact, w);
		    lv.k.fact := monicQuotient!(lv.k.fact, w);
		}
	    }
	    if not one?(lr.j.fact) then
		lret := append!(lret, lr.j);
	}
	for k in 1..#lv repeat
	    if not one?(lv.k.fact) then
		lret := append!(lret, [lv.k.fact, lv.k.exp*Pn]);
	
	return lret;
    }

#if 0
    -- berlekamp and friends crash the optimizer for -q3 and better. 

    -- u(x) is monic and squarefree.
    local QMatrixGen(u: UPf): M == {
	import from PA, M, MI, Z;
	
	local {
	    Ndimen: Z := degree(u);
	    MIdimen: MI := machine(Ndimen)$Z;
	    maxIndex: MI := prev(MIdimen);
	    arr: PA := new(MIdimen, 0$F);
	    matrix: M := new(MIdimen, nil$Pointer pretend PA);
	    t: F := 0$F;
	}
	
	arr.0 := 1;
	matrix.0 := arr;
	
	-- This is resonable for small primes only.
	for j in 0..prev(maxIndex) repeat {
	    arr := new(MIdimen, nil$Pointer pretend F);
	    
	    t := matrix.j.maxIndex;
	    for k in maxIndex..1 by -1 repeat 
		arr.k := matrix.j.(prev k) - t*coefficient(u, k::Z); 
	    arr.0 := -t*coefficient(u, 0);
	    
	    for n in 2..Pmi repeat {
		t := arr.maxIndex;
		for k in maxIndex..1 by -1 repeat 
		    arr.k := arr.(prev k) - t*coefficient(u, k::Z); 
		arr.0 := -t*coefficient(u, 0);
	    }
	    
	    matrix.(next j) := arr;
	}
	
	-- Generate Q - I.
	for j in 0..maxIndex repeat
	    matrix.j.j := matrix.j.j - 1;
	
	return matrix;
    } -- end QmatrixGen.
    
    local nullSpace!(matrix: M, MIdimen: MI): List(UPf) == {
	import from PrimitiveArray(MI), PA;
	import from MI, Z, UPf, F;
	
	maxdimen: MI := prev(MIdimen);
	c: PrimitiveArray(MI) := new(MIdimen, -1);
	vec: UPf := 0$UPf;
	ret: List(UPf) := empty;
	
	for k in 1..maxdimen repeat {
	    for j in 0..maxdimen repeat {
		if not zero?(matrix.k.j) and (c.j < 0) then {
		    elem := -inv(matrix.k.j);
		    for n in 0..maxdimen repeat
			matrix.n.j := elem*matrix.n.j;
		    for i in 0..maxdimen | (i ~= j) repeat {
			elem := matrix.k.i;
			for n in 0..maxdimen repeat
			    matrix.n.i := matrix.n.i+elem*matrix.n.j;
		    }
		    c.j := k;
		    goto ENDSCAN;
		}
	    }
	    
	    for s in 0..maxdimen repeat
		if c.s >= 0 then 
		    vec := vec + term(matrix.k.s, (c.s)::Z)$UPf;
	    vec := vec + term(1$F, k::Z)$UPf;
	    
	    ret := cons(vec, ret);
	    vec := 0;
	    
	    @ENDSCAN;
	}
	
	free!(c);
	return reverse!(ret);
    } -- end nullSpace.
    
    berlekamp(u: UPf): List(UPf) == {
	import from Z, Boolean;

	Q: M := QMatrixGen(u); 
	nSpace: List(UPf) := nullSpace!(Q, machine(degree(u)));
	fcount: MI := next(#nSpace);
	splitflag: Boolean := false;
	ret: List(UPf) := empty;
	factor: UPf := 0$UPf; 
	
	if one?(fcount) then -- u is irreducible.
	    return [u];
	
	for s in 0..prev(Pn) repeat {
	    factor := gcd(u, first(nSpace)-s::UPf);
	    if not one?(factor) then {
		fcount := prev(fcount);
		ret := append!(ret, factor);
		if zero?(fcount) then goto OUTPUT;
	    }
	}
	
	nSpace := rest(nSpace);
	for poly in nSpace repeat
	    for idx in 1..#ret repeat {
		for s in 0..prev(Pn) repeat {
		    factor := gcd(ret.idx, poly-s::UPf);
		    if not one?(factor) then {
			if splitflag then fcount := prev(fcount);
			splitflag := true;
			ret := append!(ret, factor);
			if zero?(fcount) then break;
		    }
		}
		if splitflag then {
		    ret := delete!(ret, idx);
		    if zero?(fcount) then goto OUTPUT;
		    splitflag := false;
		}
	    }
	
	@OUTPUT;
	return ret;
    } -- end berlekamp
#endif

    eea(u: UPf, v: UPf): (UPf, UPf, UPf) == {
	import from F;

	-- For what ever reason, if you dont write the 1's and
	-- 0's here fully qualified with a package call and coercion,
	-- you might not get 1's and 0's (maby 2's), and the algorithm
	-- might not be deterministic on some inputs!
	u1: UPf := (1$F)::UPf; u2: UPf := (0$F)::UPf; u3 := copy(u);
	v1: UPf := (0$F)::UPf; v2: UPf := (1$F)::UPf; v3 := copy(v);

	uc := inv(leadingCoefficient(u3));
	vc := inv(leadingCoefficient(v3));
	u3 := times!(uc, u3);
	v3 := times!(vc, v3);

	while not zero?(v3) repeat {
	    q := monicQuotient(u3, v3);
	    
	    t1 := u1 - v1*q;
	    t2 := u2 - v2*q;
	    t3 := u3 - v3*q;

	    u1 := v1; u2 := v2; u3 := v3;
	    v1 := t1; v2 := t2; v3 := t3;

	}

	ilc := inv(leadingCoefficient(u3));
	u1 := times!(ilc*uc, u1);
	u2 := times!(ilc*vc, u2);
	u3 := times!(ilc, u3);

	return (u3, u1, u2);
    }

    EEAlift(a: UPe, b: UPe, p: E, k: Z): (UPe, UPe) == {
	import from F;

	a_~: UPf := modularImage(a);
	b_~: UPf := modularImage(b);

	(g_~, s_~, t_~) := eea(a_~, b_~);
	assert(one?(g_~));

	s    := retractImage(s_~);
	t    := retractImage(t_~);
	modulus := p;

	for j in 1..prev(k) repeat {
	    e   := 1$UPe - (s*a) - (t*b);
	    c_~ := modularImage(quotient(e, modulus::UPe));
	    sigma_~ := s_~*c_~;
	    tau_~   := t_~*c_~;
	    (q_~, r_~) := monicDivide(sigma_~, b_~);
	    tau   := retractImage( (tau_~ + q_~*a_~) );
	    sigma := retractImage(r_~);
	    s := add!(s, sigma*modulus::UPe);
	    t := add!(t, tau*modulus::UPe);
	    modulus := times!(modulus, p);
	}
	return (s, t);
    }
}
