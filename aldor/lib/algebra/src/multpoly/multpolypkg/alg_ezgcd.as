#include "algebra"

macro {
    Z  == Integer;
    MI == MachineInteger;
}

MultivariateGCD( R: Join(GcdDomain,ModularComputation,SourceOfPrimes), 
		 V: FiniteVariableType, 
		 MP: PolynomialRing(R, V) ): with {

    ezGCD: (MP, MP, R) -> (MP, MP, MP);
    ++ ezGCD(a,b,l) implements multivariate polynomial GCD
    ++ computations using modular techniques. `a' and `b' are
    ++ arbitrary multivariate polynomials. `l' is a bound on the
    ++ magnitues of all coefficients of `a' and `b' as well as the
    ++ coefficients of the gcd to be computed. 


} == add {

    macro {
	UP  == SparseUnivariatePolynomial(R);
	EVA == Array Cross(V, R);
    }

    if R has EuclideanDomain then
	modP(p: R): R -> R == (c: R): R +-> symmetricMod(c, p);
    else
	modP(p: R): R -> R == (c: R): R +-> (c mod p);

    local MPmodP!(f: MP, p: R): MP == map!(modP(p))(f);
    local MPmodP!(f: UP, p: R): UP == map!(modP(p))(f);

    local canonical?: Boolean == canonicalUnitNormal?$R;
    local euclidean?: Boolean == R has EuclideanDomain;


    local safeQuotient(a: MP, b: MP): MP == {
	import from Partial MP;
	res? := exactQuotient(a, b);
	assert(not failed? res?);
	res  := retract res?;
	return res;
    }
    local safeQuotient(a: UP, b: UP): UP == {
	import from Partial UP;
	res? := exactQuotient(a, b);
	assert(not failed? res?);
	res  := retract res?;
	return res;
    }

    asMultivariate(f: UP, x: V): MP == {
	local res: MP := 0;
	for t in f repeat {
	    (c, n) := t;
	    res := add!(res, term(c, x, n));
	}
	return res;
    }

    local asUnivariate(f: MP, x: V): UP == {
	local res: UP := 0;
	for t in support(f) repeat {
	    (c, base) := t;
	    res := add!(res, term(c, degree(base, x)));
	}
	return res;
    }

    -- sortByDegree(a, b) returns (x, u, v, f) such that `x' is the
    -- variable from `a' or `b' with maximum degree, and degree(u,x)
    -- >= degree(v,x). `f' is a Boolean indicating if a swap occurred
    -- (i.e. if `b'=`u' then true, else `a'=`u' thus false). This
    -- helps simplify the main ezGCD routine by sorting the input
    -- polynomials w.r.t degree.
    local sortByDegree(a: MP, b: MP): (V, MP, MP, Boolean) == {
	import from Z;
	
	local {
	    x:  V := mainVariable(a);
	    y:  V := mainVariable(b);
	    xd: Z := degree(a, x);
	    yd: Z := degree(b, y);
	}
	if xd >= yd then
	    return (x, a, b, false);
	return (y, b, a, true);
    }

    -- primitiveMP(a,x,bound) returns (c,pp) where `c' is the 
    -- content of `a' and `pp' is the primitive part of `a'. `a' 
    -- is viewed as a univariate polynomial in the indeterminate 
    -- `x'. `bound' is passed to ezGCD. 
    local primitiveMP(a: MP, x: V, pbound: R): (MP, MP) == {
	import from UP, Z;
	local {
	    cont:  MP;
	    ppart: MP;
	    mp:    MP;
	}

	trail := coefficient(a,x,0);
	if ( (ground?(trail) and not zero?(trail)) or 
	     (univariate? a and x = mainVariable(a)) )  
	then {
	    (contx, ppart) := primitive(a);
	    cont := contx::MP;
	    return (cont, ppart);
	} else {
	    cont := leadingCoefficient(a, x);
	    if ground? cont then {
		(contx, ppart) := primitive(a);
		cont := contx::MP;
		return (cont, ppart);
	    } else {
		ard := a - cont*(x::MP)^degree(a,x);
		for mpz in combine(ard, x) repeat {
		    (mp, z) := mpz;
		    if ground? mp then {
			(contx, ppart) := primitive(a);
			cont := contx::MP;
			return (cont, ppart);
		    }
		    (cont, t1, t2) := ezGCD(cont, mp, pbound);
		    if ground? cont then {
			(contx, ppart) := primitive(a);
			cont := contx::MP;
			return (cont, ppart);
		    }
		}
	    }
	}

	ppart := safeQuotient(a, cont);
	return (cont, ppart);
    }

          -- `combine(p,v)' generates only the couple `(0,p)' if
          -- `ground?(p)' holds otherwise generates `(n,an)...(m,am)'
          -- such that `p' writes `an*v^n+...+am*v^m' as a univariate
          -- polynomial in the variable `v'. Couples in the output
          -- are sorted by (strictly) increasing degree w.r.t. `v'.
    local combine(ard: MP, x: V): Generator Cross(MP, Z) == never;

    -- findPrime(a,b,x,p) returns a prime `P' such that the leading
    -- coefficients of `a' and `b' mod `P' w.r.t the indeterminate `x'
    -- are not zero. `p', a prime, is an optional argument which is
    -- used to compute the successor prime `P'.
    local findPrime(a: MP, b: MP, x: V, p: R == 0): R == {
	import from Partial R;
	local ret?: Partial R;
	if zero? p then 
	    ret? := getPrime();
	else 
	    ret? := nextPrime(p);
	assert(not failed? ret?);
	ret: R := retract ret?;
	lca := leadingCoefficient(a, x);
	lcb := leadingCoefficient(b, x);
	while zero?(MPmodP!(lca, ret)) or zero?(MPmodP!(lcb, ret)) repeat {
	    ret? := nextPrime(ret);
	    assert(not failed? ret?);
	    ret: R := retract ret?;
	}
	return ret;
    }

    -- makeVarList(a, b) computes `variables(a) union variables(b)'.
    local makeVarList(a: MP, b: MP): List V == {
	ret: List V := [variables a];
	for v in variables b repeat 
	    if not member?(v, ret) then 
		ret := cons(v, ret);
	return ret;
    }

    -- getEvalArray(a,b,x,p,pre) computes an array `res' of pairs (v,
    -- r) where `v' is an indeterminate of `a' or `b', and `r' is an
    -- evaluation point for `v'. Each such `b' satisfies `0 <= b <
    -- p'. Furthermore, leadingCoefficient(a,x)(res) ~= 0 and
    -- leadingCoefficient(b,x)(res) ~= 0. `pre' is an optional
    -- argument which, when given, is used to compute a successor
    -- evaluation array `res'.
    local getEvalArray(a: MP, b: MP, x: V, p: R, pre: EVA == empty): EVA == {
	import from List V, R, MI;
	
	local {
	    ret: EVA;
	    lca    := leadingCoefficient(a, x);
	    lcb    := leadingCoefficient(b, x);
	    j: MI  := 0;
	    mj: MI := prev(#pre);
	}

	if empty? pre then {
	    vlist := makeVarList(a, b);
	    ret := new(prev(#vlist));
	    for v in vlist | v ~= x repeat {
		ret.j := (v, 0);
		j := next j;
	    }
	    j  := 0;
	    mj := prev(#ret);
	} else {
	    ret := copy(pre);
	    i: MI := abs(random());
	    i := i rem next(mj);
	    (v, e) := ret.i;
	    ret.i  := (v, ((e+1) mod p));
	}
	repeat {
	    eva := lca;
	    evb := lcb;
	    for ev in ret repeat {
		(v, e) := ev;
		eva := eval(eva, v, e);
		evb := eval(evb, v, e);
		if zero?(eva) or zero?(evb) then break;
	    }
	    if zero?(eva) or zero?(evb) then {
		if j > mj then j := 0;
		(v, e) := ret.j;
		ret.j  := (v, ((e+1) mod p));
		j := next j;
	    } else break;
	}
	return ret;
    }

    -- sGCD is an extension to the main ezGCD algorithm to deal with
    -- the special case when the modular cofactors are not relatively
    -- prime to the modular gcd. In this case, sGCD attempts to solve
    -- the congruence `q = gx*(q/gx) mod <p, I>' where q is of the
    -- form `s*ax+t*bx'. Here, `gcd(ax,bx)=gx' and `s', `t' are chosen
    -- such that `gcd((s*ax+t*bx)/gx, gx)=1'. 
    local sGCD(a: MP, b: MP, g: MP, ax: UP, bx: UP, conta: MP, contb: MP, 
	       gx: UP, evArr: EVA, x: V, p: R, pbound: R ): Partial Cross(MP, MP, MP) == {
	import from Z, Partial MP;
	local {
	    s: Z   := 1;
	    t: Z;
	    flag: Boolean := true;
	    c:  MP := conta*contb;
	    sx: UP;
	    tx: UP;
	    hx: UP;
	    qx: UP;
	}

	while flag repeat {
	    s  := next s;
	    sx := s*ax;
	    t  := 1;
	    while t <= s repeat {
		tx := t*bx;
		hx := sx + tx;
		qx := quotient(hx, gx);
		if one?(gcd(qx, gx)) then {
		    flag := false;
		    break;
		}
		t := next t;
	    }
	}

	u := c*(s*a+t*b);
	h := asMultivariate(qx, x);
	(r1, r2) := doLift(u, h, c, asMultivariate(gx, x), evArr, x, p, pbound);
	if u = r1*r2 then return failed;

	(forget, r1) := primitiveMP(r1, x, pbound);
	div1? := exactQuotient(a, r1);
	if failed? div1? then return failed;
	div2? := exactQuotient(b, r1);
	if failed? div2? then return failed;
	div1  := retract div1?;
	div2  := retract div2?;
	r2 := times!(conta,div1);
	r3 := times!(contb,div2);
	g  := times!(g,r1);

	return [(g, r2, r3)];
    }

    -- univariateGCD(a,b,x) performs a gcd computation on `a' and `b',
    -- where `a' and `b' are strictly univariate in the indeterminate
    -- `x'. This procedure returns (g,u,v) where `g=gcd(a,b)',
    -- `u=(a/g)', and `v=(b/g)'.
    local univariateGCD(a: MP, b: MP, x: V): (MP, MP, MP) == {
	import from UP;

	ax := asUnivariate(a, x);
	bx := asUnivariate(b, x);
	gx := gcd(ax,copy(bx));
	ax := quotient!(ax, copy(gx));
	bx := quotient!(bx, copy(gx));
	aa := asMultivariate(ax,x);
	bb := asMultivariate(bx,x);
	g  := asMultivariate(gx,x);
	return (g, aa, bb);
    }

    -- `trvialCases(a,b)' checks for and solves the following simple
    -- gcd situations: 
    -- 1) Equality between `a' and `b'; 
    -- 2) The univarite gcd problem.  
    -- 3) If either `a' or `b' are ground elements.
    local trivialCases(a: MP, b: MP): Partial Cross(MP,MP,MP) == { 
	import from R, V;
	if a = b then return [(copy(a), 1, 1)];
	if univariate? a and univariate? b then {
	    x := mainVariable(a);
	    if x = mainVariable(b) then 
		return [univariateGCD(a, b, x)];
	}
	if ground? a then {
	    ar := leadingCoefficient(a);
	    if ground? b then {
		br := leadingCoefficient(b);
		(gr, ar, br) := gcdquo(ar, br);
		return [(gr::MP, ar::MP, br::MP)];
	    } else {
		(br, bb) := primitive(b);
		(gr, ar, br) := gcdquo(ar, br);
		bb := times!(br,bb);
		return [(gr::MP, ar::MP, bb)];
	    }
	} else if ground? b then {
	    br := leadingCoefficient(b);
	    (ar, aa) := primitive(a);
	    (gr, ar, br) := gcdquo(ar, br);
	    aa := times!(ar, aa);
	    return [(gr::MP, aa, br::MP)];
	}
	return failed;
    }

    -- See domain description. 
    ezGCD(aa: MP, bb: MP, pbound: R): (MP, MP, MP) == {

	import from EVA, Cross(R, V), UP;
	import from Partial MP, Partial UP, Z, V;
	import from Partial Cross(MP, MP, MP);

	local {
	    g: MP;
	    conta: MP;
	    contb: MP;
	}

	trivial? := trivialCases(aa,bb);
	if not failed? trivial? then {
	    (r1,r2,r3) := retract trivial?;
	    return (r1,r2,r3);
	}

	(x, a, b, swap?) := sortByDegree(copy(aa), copy(bb));
	p := findPrime(a, b, x);
	(conta, a) := primitiveMP(a, x, pbound);
	(contb, b) := primitiveMP(b, x, pbound);

	trivial? := trivialCases(conta, contb);
	if failed? trivial? then
	    (g, conta, contb) := ezGCD(conta,contb,pbound);
	else
	    (g, conta, contb) := retract trivial?;

	evArr := getEvalArray(a, b, x, p);

	coa   := a;
	cob   := b;
	for ev in evArr repeat {
	    (v, e) := ev;
	    coa    := eval(coa, v, e);
	    cob    := eval(cob, v, e);
	}
	ax   := asUnivariate(MPmodP!(coa, p), x);
	bx   := asUnivariate(MPmodP!(cob, p), x);
	gx   := gcd(ax, copy(bx));
	d: Z := degree(gx);
	
	if zero? d then {
	    r1 := conta*a;
	    r2 := contb*b;
	    (g, r1, r2) := makeNormal(g, r1, r2);
	    if swap? then 
		return (g, r2, r1);
	    else
		return (g, r1, r2);
	}

	p_'     := p;
	evArr_' := evArr;
	repeat {
	    p_'     := findPrime(a, b, x, p_');
	    evArr_' := getEvalArray(a, b, x, p_', evArr_');

	    coa_'   := a;
	    cob_'   := b;
	    for ev in evArr_' repeat {
		(v, e) := ev;
		coa_'  := eval(coa_', v, e);
		cob_'  := eval(cob_', v, e);
	    }
	    ax_' := asUnivariate(MPmodP!(coa_', p_'), x);
	    bx_' := asUnivariate(MPmodP!(cob_', p_'), x);
	    gx_' := gcd(ax_', copy(bx_'));
	    d_'  := degree(gx_');

	    if d_' < d then {
		ax := ax_';
		bx := bx_';
		gx := gx_';
		d  := d_';
		free!(evArr);
		evArr := evArr_';
		iterate;
	    } 
	    if d_' > d then iterate;

	    if zero? d then {
		r1 := times!(conta,a);
		r2 := times!(contb,b);
		(g, r2, r2) := makeNormal(g, r1, r2);
		if swap? then
		    return (g, r2, r1);
		else 
		    return (g, r1, r2);
	    }
	    if d = degree(b, x) then {
		divides? := exactQuotient(a, b);
		if not failed? divides? then {
		    q  := retract divides?;
		    r1 := times!(q, conta);
		    r2 := times!(g, b);
		    (r2, r1, contb) := makeNormal(r2, r1, contb);
		    if swap? then return (r2, contb, r1);
		    return (r2, r1, contb);
		}
		d := prev(d);
		iterate;
	    }

	    coprimeCofactors? := prepareCofactors(ax,bx,gx,a,b,conta,contb,x);

	    if failed? coprimeCofactors? then {
 		res? := sGCD(a,b,g,ax,bx,conta,contb,gx,evArr,x,p,pbound);
		if failed? res? then iterate;
		(r1,r2,r3) := retract res?;
		(r1,r2,r3) := makeNormal(r1,r2,r3);
		if swap? then 
		    return (r1,r3,r2);
		return (r1,r2,r3);
	    }

	    (u,h,c)  := retract coprimeCofactors?;
	    (r1, r2) := doLift(u, h, c, asMultivariate(gx, x), evArr, x, p, pbound);

	    if u ~= r1*r2 then iterate;

	    (forget, r1) := primitiveMP(r1, x, pbound);
	    div1?  := exactQuotient(a, r1);
	    if failed? div1? then iterate;
	    div2?  := exactQuotient(b, r1);
	    if failed? div2? then iterate;
	    div1   := retract div1?;
	    div2   := retract div2?;

	    r2 := times!(conta,div1);
	    r3 := times!(contb,div2);
	    g  := times!(g,r1);
	    (g,r2,r3) := makeNormal(g,r2,r3);

	    if swap? then return (g, r3, r2);
	    return (g, r2, r3);
	}
    }

    local prepareCofactors( ax: UP, bx: UP, gx: UP, a: MP, b: MP, 
			    conta: MP, contb: MP, x: V ): Partial Cross(MP,MP,MP) == {
	
	local { u : MP; h: MP; c: MP; qx: UP }
	    
	qx := safeQuotient(bx, gx);
	if one?(gcd(qx,gx)) then {
	    u := b*contb;
	    h := asMultivariate(qx, x);
	    c := copy contb;
	} else {
	    qx := safeQuotient(ax, gx);
	    if one?(gcd(qx, gx)) then {
		u := a*conta;
		h := asMultivariate(qx, x);
		c := copy conta;
	    } else return failed;
	} 

	return [(u,h,c)];
    }


    -- doLift is an extension to the main ezGCD algorithm which
    -- performs some preparatory work before hensel lifting is
    -- invoked.
    local doLift(u: MP, h: MP, c: MP, g: MP, 
		 evArr: EVA, x: V, p: R, pbound: R ): (MP, MP) == {
	import from Array MP, Z, MI;
	import from MultivariateHenselLifting( R, V, MP );

	cx := copy(c);
	for ev in evArr repeat {
	    (v, e) := ev;
	    cx := eval(cx, v, e);
	}
	if euclidean? then 
	    pbound := 2*pbound;

	cx     := MPmodP!(cx, p);
	g      := times!(cx, g);
	mdeg   := getMaxDeg(u, evArr);
	(bound, tmp:R) := orderquo(p+1)(pbound);

	lArr   := lift(u, evArr, mdeg, p, next(bound), [g, h], [c, c]);

	return (lArr.0, lArr.1);
    }
    
    -- getMaxDeg(u, e) is a helper function for `doLift'. It finds the
    -- maximum degree of `u' w.r.t the variables mentioned in the
    -- evaluation array `e'. 
    local getMaxDeg(u: MP, evArr: EVA): Z == {
	import from Cross(V, R);
	ret: Z := 0;
	for va in evArr repeat {
	    (v, a) := va;
	    tmp := degree(u, v);
	    if tmp > ret then ret := tmp;
	}
	return ret;
    }

    -- makeNormal(g,a,b) attempts to provide a canonical
    -- representative for the gcd `g', adjusting `g', `a', and `b'
    -- via multiplication by a unit.
    local makeNormal(g: MP, a: MP, b: MP): (MP, MP, MP) == {
	import from R;
	if not canonical? then
	    return (g,a,b);
	lc := leadingCoefficient(g);
	if one? lc then return (g,a,b);
	(lc_', u, u_') := unitNormal(lc);
	g := times!(u_', g);
	a := times!(u_', a);
	b := times!(u_', b);

	return (g,a,b);
    }
}


-- #if ALDORTEST

-- #include "algebra"
-- #include "aldortest"


-- macro {
--     INT   == Integer;
--     MI    == MachineInteger;
--     OVL   == OrderedVariableList(symList);
--     SMP   == SparseIntegerMultivariatePolynomial(OVL);
--     POL   == SMP;
-- }

-- import from Symbol, List Symbol;
-- W: Symbol == -"w";
-- X: Symbol == -"x";
-- Y: Symbol == -"y";
-- Z: Symbol == -"z";

-- symList : List Symbol == [W, X, Y, Z];

-- makePolys(): (POL, POL) == {
--     import from INT, OVL;

--     w: POL := variable(machine(1))::POL;
--     x: POL := variable(machine(2))::POL;
--     y: POL := variable(machine(3))::POL;
--     z: POL := variable(machine(4))::POL;

--     co1: POL := y + 1;
--     co2: POL := y - 1;
--     co3: POL := y^3 - 1;
--     co4: POL := z + 1;
--     co5: POL := z - 1;
--     co6: POL := z^3 - 1;
--     co7: POL := w^7*y*z^2-w^2*x+w;
--     b1: POL := co1*co2*(co6-co5)*co3*co2*co5*co6*co7;
--     b2: POL := co2*co5*co7;
--     a: POL := b1*x^2 + b2*x + (co5-co1)*co5*co2*co7;
--     b: POL := (co5*co6*(co2-co4)*co3^3*co7)*x^7 + co1*co2*co7*x^2+co2*(co5-co4)*co5*co7;
    
--     return (a, b);
-- }

-- ezgcd__test(): Boolean == {
--     import from MultivariateGCD(INT, OVL, POL), INT, POL;
--     (a, b) := makePolys();

--     (q, r1, r2) := ezGCD(a, b, 1);

--     if (q*(r1+r2)) = (a+b) then {
--  	(q, r1, r2) := ezGCD(r1, r2, 1);
--  	return one?(q);
--     }
--     return false;
-- }

-- stdout << "test ezgcd.as .... " << newline;
-- aldorTest("GCD in Z[w,x,y,z]", ezgcd__test);
-- stdout << newline;

-- #endif
