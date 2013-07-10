#include "algebra"

macro {
    Z  == Integer;
    MI == MachineInteger;
    MP == PolynomialRing;
    MPARR == Array MPc;
    MPMAT == Array MPARR;
    UPc   == SparseUnivariatePolynomial(CR);
    UPARR == Array UPc;
}

MultivariateDiophantineEquationSolver( CR  : Join(ModularComputation, IntegralDomain),
				       V   : VariableType,
				       MPc : MP(CR, V) ): with {
    
    multivariateDiophant: (Array MPc, MPc, Array Cross(V, CR), Z, CR, Z) -> Array MPc; 
    ++ multivariateDiophant(a, c, id, d, p, k) solves in 
    ++ `CR_(p^k)[x,x_1,..x_n]' the polynomial diophantine equation 
    ++ `s_0*b_0+ ... +s_r*b_r = c (mod< id^d+1, p^k>)', where the `b_i's
    ++ are defined by `b_i = a[0]*...a[i-1]*a[i+1]*...*a[r]'. 
    ++
    ++ `id' is a list of equations { x_1 = t_1, ..., x_n = t_n } which 
    ++ is interpeted mathematicly as an ideal. 
    ++
    ++ 
    ++ `p' must not divide leadingCoefficient(a_i mod id) for all i. 
    ++ `a_i mod<id, p>' must me relatively prime in CR_p[x] ( where 
    ++ the special indeterminate `x' is deduced from it not being 
    ++ present in the list of equations given in `id' ). Also, 
    ++ degree(c, x) < sum(degree(a_i, x), i=1..r);

    multivariateDiophant: (MPc,MPc,MPc,UPc,UPc,Array Cross(V, CR),Z,CR)  -> Array MPc;
    ++ muntivariateDiophant(a,b,c,s,t,id,d,pk) solves the polynomial 
    ++ diophantine equation `g*a+h*b = c (mod< id^(d+1), pk>)'.
    ++
    ++ `pk' = `p^k' for some prime `p' and integer `k'.
    ++
    ++ `s' and `t' are the bezout coefficients of `a' and `b' satisfying 
    ++ `s*a+t*b = 1 (mod pk)'. 
    ++
    ++ `id' is a list of equations { x_1 = t_1, ..., x_n = t_n } which
    ++ is interpeted mathematicly as an ideal. 
    ++
    ++ `a' and `b' must be relatively prime in CR_p[x] ( where the special
    ++ indeterminate `x' is deduced from it not being present in the list 
    ++ of equations given in `id'). Also, `degree(c, x) < degree(a, x) +
    ++ degree(b, x)'.
    
} == add {


    if CR has EuclideanDomain then
	modP(p: CR): CR -> CR == (c: CR): CR +-> symmetricMod(c, p);
    else
	modP(p: CR): CR -> CR == (c: CR): CR +-> (c mod p);

    local MPmodP!(f: MPc, p: CR): MPc == map!(modP(p))(f);
    local MPmodP!(f: UPc, p: CR): UPc == map!(modP(p))(f);
	
    local taylorCoeff(f: MPc, x: V, a: CR, n: Z): MPc == {
	local {
	    m:   MPc;
	    q:   MPc;
	    r:   MPc;
	}
	    
	m := x::MPc - a::MPc;
	(q, r) := monicDivide(f, m, x);
	zero? n => return eval(r, x, a);
	for i in 1..n repeat {
	    if zero? q then return 0;
	    (q, r) := monicDivide(q, m, x);
	}
	return eval(r, x, a);
    }

    asMultivariate(f: UPc, x: V): MPc == {
	local res: MPc := 0;
	for t in f repeat {
	    (c, n) := t;
	    res := add!(res, term(c, x, n));
	}
	return res;
    }

    local asUnivariate(f: MPc, x: V): UPc == {
	local res: UPc := 0;
	for t in support(f) repeat {
	    (c, base) := t;
	    res := add!(res, term(c, degree(base, x)));
	}
	return res;
    }
    local asUnivariate(a: MPARR, x: V): UPARR == {
	import from MI;
	local {
	    r:    MI    := #a;
	    rbnd: MI    := prev(r);
	    res:  UPARR := new(r);
	}
	for i in 0..rbnd repeat 
	    res.i := asUnivariate(a.i, x);
	return res;
    }

    local asMultivariate(a: UPARR, x: V): MPARR == {
	import from MI, MPc;
	local {
	    r:    MI    := #a;
	    rbnd: MI    := prev(r);
	    res:  MPARR := new(r);
	}
	for i in 0..rbnd repeat
	    res.i := asMultivariate(a.i, x);
	return res;
    }

    local buildTables(a: MPARR, id: Array Cross(V, CR)): (MPMAT, UPARR, V) == {
	import from MI, MPc;

	local { 
	    vbnd: MI := prev(#id);
	    rbnd: MI := prev(#a);
	    aVec: MPARR := a;
	    bMat: MPMAT := new(#id);
	}

	for i in vbnd..0 by -1  repeat {
	    (x, at) := id.i;
	    bMat.i  := buildProductArray(aVec);
	    for j in 0..rbnd repeat 
		aVec.j := eval(aVec.j, x, at);
	}
	x := findVariable(aVec, 1);
	return (bMat, asUnivariate(aVec, x), x);
    }

    local buildProductArray(a: MPARR): MPARR == {
	import from MI;
	
	local {
	    r: MI := #a;
	    rbnd: MI := prev(r);
	    b: Array MPc := new(r);
	    worker: MPc;
	}

	if r = 2 then {
	    b.0 := a.1;
	    b.1 := a.0;
	} else {
	    worker := 1;
	    for i in 1..rbnd repeat 
		worker := times!(worker, a.i);
	    b.0 := worker;
	    
	    worker := 1;
	    for i in 1..prev(rbnd) repeat {
		for j in 0..prev(i) repeat
		    worker := times!(worker, a.j);
		for j in next(i)..rbnd repeat
		    worker := times!(worker, a.j);
		b.i := worker;
	    }
	    
	    worker := 1;
	    for i in 0..prev(rbnd) repeat
		worker := times!(worker, a.i);
	    b.rbnd := worker;
	}
	return b
    }

    -- Specialization
    multivariateDiophant( a: MPc, b: MPc, c: MPc, s: UPc, t: UPc,
			  id: Array Cross(V, CR), d: Z, pk: CR ): Array MPc == {
	import from Array MPc, Array UPc;

	local {
	    sigma: Array MPc;
	    x: V;
	    aSpec: UPARR;
	    bMat:  MPMAT;
	    bezco: UPARR := [s, t];
	}

	if empty? id then {
	    local ma : MPARR := [a, b];
	    x     := findVariable(ma, c);
	    aa    := asUnivariate(ma, x);
	    cc    := asUnivariate(c, x);
	    sigma := asMultivariate(univariateUpdate(aa,cc,bezco,pk),x);
	} else {
	    (bMat, aSpec, x) := buildTables([a,b], id);
	    sigma := multiUpdate(aSpec,copy(c),id,x,bMat,bezco,d,pk,#id);
	}
	return sigma;
    }
    
    multivariateDiophant(a: Array MPc, c: MPc, id: Array Cross(V, CR),
		       d: Z, p: CR, k: Z): Array MPc == {
	import from MI;

	local {
	    sigma: Array MPc;
	    x: V;
	    pk: CR := p^k;
	    aSpec: UPARR;
	    bMat:  MPMAT;
	    bezco: UPARR;
	}
	
	-- First consider the univariate case.
	if empty? id then { 
	    x     := findVariable(a, c);
	    aa    := asUnivariate(a, x);
	    cc    := asUnivariate(c, x);
	    bezco := bezoutIntern(aa, p, k);
	    sigma := asMultivariate(univariateUpdate(aa, cc, bezco, pk), x);
	} else {
	    (bMat, aSpec, x) := buildTables(copy(a), id);
	    bezco := bezoutIntern(aSpec, p, k);
	    sigma := multiUpdate(aSpec,copy(c),id,x,bMat,bezco,d,pk,#id);
	}
	return sigma;
    }

    local univariateUpdate(a: UPARR, c: UPc, 
			   bezco: UPARR, pk: CR): UPARR == {
	import from MI;
	local {
	    r:    MI := #a;
	    rbnd: MI := prev(r);
	    x: UPc   := monom;
	    sigma: UPARR := new(r, 0);
	}
	assert(r > 1);
	assert(r = #bezco);

	for t in c repeat {
	    (cm, m) := t;

	    if r > 2 then {
		for i in 0..rbnd repeat {
		    (q, u) := quoRemMod(x, m, bezco.i, a.i, pk); 
		    sigma.i := add!(sigma.i, cm*u);
		}
	    } else {
		xm := x^m;
		(q, u) := quoRemMod(x, m, bezco.0, a.0, pk);
		sigma.0 := add!(sigma.0, cm*u);
		sigma.1 := add!(sigma.1, (cm*(xm*bezco.1 + q*a.1)));
	    }
	}
	for i in 0..rbnd repeat 
	    sigma.i := MPmodP!(sigma.i, pk);

	return sigma;
    }

    local multiUpdate(a: UPARR, c: MPc, id: Array Cross(V, CR), x: V,
		      bMat: MPMAT, bezco: UPARR, d: Z, pk: CR, j: MI): MPARR == {
	import from MI;
	local {
	    r: MI    := #a;
	    v: MI    := #id;
	    rbnd: MI := prev(r);
	    vbnd: MI := prev(v);
	    cSub: MPc;
	    err:  MPc;
	    hand: MPc;
	    mpow: MPc := 1;
	    monomial: MPc;
	    sigma: MPARR;
	}
	assert(r > 1); assert(j >= 0);

	if zero?(j) then {
	    local {
		cc: UPc := asUnivariate(c, x);
	    }
	    return asMultivariate(univariateUpdate(a,cc,bezco,pk),x);
	} else {
	    j         := prev(j);
	    (var, at) := id.j;
	    monomial  := (var::MPc - at::MPc);
	    cSub      := eval(c, var, at);
	    sigma     := multiUpdate(a,cSub,id,x,bMat,bezco,d,pk,j);
	    
	    hand  := sigma.0*bMat.j.0;
	    for i in 1..rbnd repeat
		hand := add!(hand, sigma.i*bMat.j.i);
	    err := c - hand;
	    err := MPmodP!(err, pk);
	    
	    for m in 1..d repeat {
		if zero?(err) then break;

		mpow := times!(mpow, monomial);
		tc   := taylorCoeff(err, var, at, m);
		
		if not zero?(tc) then {
		    dsigma := multiUpdate(a,tc,id,x,bMat,bezco,d,pk,j);
		    hand   := 0;
		    for i in 0..rbnd repeat {
			dsigma.i := times!(dsigma.i, mpow);
			sigma.i  := add!(sigma.i, dsigma.i);
			hand     := add!(hand, dsigma.i*bMat.j.i);
		    }
		    err := err - hand;
		    err := MPmodP!(err, pk);
		    free!(dsigma);
		}
	    }
	}

	for i in 0..rbnd repeat 
	    sigma.i := MPmodP!(sigma.i, pk);
	return sigma;
    }

    -- bezoutIntern(a, p, k) computes s_1,...s_r such that
    -- s_1*b_1+...s_r*b_r = 1 mod p^k, degree(s_j) < degree(a_j), and
    -- where the b_j are defined by 
    -- b_j = a_1*..*a_(j-1)*a_(j+1)*..*a_r. 
    local bezoutIntern(a: UPARR, p: CR, k: Z): UPARR == {
	import from MI, MPc;

	local {
	    r:    MI := #a;
	    rbnd: MI := prev(r);
	    p_^k: CR := p^k;
	    s:    UPARR;
	}
	assert(r >= 2);

	if r > 2 then {
	    s := multiTermEEAlift(a, p, k);
	} else {
	    s        := new(2);
	    (s0, s1) := doEEAlift(a.1, a.0, p, k);
	    s.0      := s0;
	    s.1      := s1;
	}

	return s;
    }


    -- findVariable(a, c) checks that the elements of the array `a'
    -- and the polynomial `c' are univariate polynomials in the same
    -- indeterminate. `a' may contain constant polynomials, but at
    -- least one must have degree > 0. The polynomial `c' may be
    -- constant. findVariable returns this unique indeterminate. 
    local findVariable(a: Array MPc, c: MPc): V == {
	import from Z;

	local {
	    x: V;
	    foundOne: Boolean := false;
	}

	for t in a repeat {
	    if univariate? t then
		if foundOne then 
		    assert(x = mainVariable(t));
		else {
		    x := mainVariable(t);
		    foundOne := true;
		}
	    else assert(ground?(t));
	}
	assert(foundOne);

	if univariate? c then
	    assert(x = mainVariable(c));
	else assert(ground?(c));

	return x;
    }

    -- x^m*s = b*q+r
    local quoRemMod(x: UPc, m: Z, s: UPc, b: UPc, p: CR): (UPc, UPc) == {
	
	bdeg: Z := degree(b);
	sdeg: Z := degree(s);
	if m+sdeg < bdeg then 
	    return (0, (x^m)*s);
	
	n := bdeg-sdeg-1;
	q : UPc := 0;
	r : UPc := (x^n)*s;

	cob: CR := leadingCoefficient(b);
	co_~: CR := invMod(cob, p);

	while n ~= m repeat {
	    if degree(r) < prev(bdeg) then {
		q := x*q;
		r := x*r;
		n := next n;
		iterate;
	    }

	    cor: CR := leadingCoefficient(r);
	    q := MPmodP!( (x*q)+(cor*co_~)::UPc, p );
	    r := MPmodP!( (x*r)-(cor*co_~*b), p );

	    n := next n;
	}
	return (q, r);
    }

    multiTermEEAlift(a: UPARR, p: CR, k: Z): UPARR == {
	import from MI, UPc;
	local {
	    r:     MI := #a;
	    rbnd:  MI := prev r;
	    pk:    CR := p^k;
	    q:     UPARR := new(rbnd);
	    s:     UPARR := new(r);
	    beta:  UPARR := new(r);
	    sigma: UPARR;
	    pair:  UPARR;
	    bezco: UPARR;
	}
	
	q.(prev rbnd) := a.rbnd;
	for j in (rbnd-2)..0 by -1 repeat {
	    nj  := next(j);
	    q.j := a.nj*q.nj;
	}
	beta.0 := 1;

	for j in 0..(prev rbnd) repeat {
	    nj := next j;
	    pair  := [q.j, a.j];
	    bezco := bezoutIntern(pair, p, k);
	    sigma := univariateUpdate(pair, beta.j, bezco, pk);
	    beta.nj := sigma.0;
	    s.j     := sigma.1;
	    free!(pair); free!(sigma); free!(bezco);
	}
	s.rbnd := beta.rbnd;
	free!(q); free!(beta); 
	return s;
    }

    local doEEAlift(a: UPc, b: UPc, p: CR, k: Z): (UPc, UPc) == {
	import from UPARR;

	F == residueClassRing(p);
	import from F;
	macro {
	    UPf  == DenseUnivariatePolynomial(F);
	    MUPF == ModularUnivariateTools(CR, UPc, F, UPf, modularRep, canonicalPreImage);
	}
	import from MUPF;

	(s, t) := EEAlift(a, b, p, k);

	return (s, t);
    }

}
    
    
