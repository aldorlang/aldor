#include "algebra"

macro {
    Z  == Integer;
    MI == MachineInteger;
    MP == PolynomialRing;
}

MultivariateHenselLifting( CR  : Join(ModularComputation, IntegralDomain),
			   V   : VariableType,
			   MPc : MP(CR, V) ): with {
    
    lift: (MPc, Array Cross(V, CR), Z, CR, Z, Array MPc, Array MPc) -> Array MPc; 
    ++ lift(a,id,mdeg,p,l,u,uc), where `a=u[1]*...*u[n] mod <p, id>',
    ++ `uc' is an array of the true leading coefficients of the
    ++ `u[i]', `mdeg' is the maximum degree of any monomial appearing
    ++ in `a' or any of its factors, and `p^l' bounds the magnitude of
    ++ any of the coefficients of `a' or any of its factors (in the
    ++ case the coefficient ring is an Euclidean domain, then `p^l'
    ++ must bound twice the magnitudes of the coefficients as a
    ++ symmetric modular representation will be assumed). `lift'
    ++ returns an array `v' such that `a=v[1]*...*v[n] mod p^l'.

    
} == add {

    if CR has EuclideanDomain then
	modP(p: CR): CR -> CR == (c: CR): CR +-> symmetricMod(c, p);
    else
	modP(p: CR): CR -> CR == (c: CR): CR +-> (c mod p);

    local MPmodP!(f: MPc, p: CR): MPc == map!(modP(p))(f);
	
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

    local findMainVariable(u: Array MPc): V == {
	import from MPc;
	local x: V;
	foundOne: Boolean := false;
	for t in u repeat {
	    if univariate? t then 
		if foundOne then
		    assert(x = mainVariable(t));
		else {
		    x := mainVariable(t);
		    foundOne := true;
		}
	    else assert(ground? t);
	}
	assert(foundOne);
	return x;
    }

    -- Further input:
    -- Array of bezout coefficients.
    lift(a: MPc, I: Array Cross(V, CR), mdeg: Z, p: CR, l: Z, u: Array MPc, uc: Array MPc): 
	Array MPc == {
	import from MI, V, MultivariateDiophantineEquationSolver(CR,V,MPc);

	local {
	    v: MI        := next(#I);
	    n: MI        := #u;
	    nbnd: MI     := prev(n);
	    vbnd: MI     := prev(v);
	    A: Array MPc := new(v, 0);
	    p_^l: CR     := p^l;
	    U: Array MPc := copy(u);
	    U1: Array MPc;
	    II: Array Cross(V, CR) := empty;
	    deltaU: Array MPc;
	    mainvar: V := findMainVariable(u);
	    mainmonom: MPc := mainvar::MPc;
	    mpow:   MPc;
	    monom:  MPc;
	    worker: MPc;
	    e: MPc;
	}
	assert(n = #uc);

	A.vbnd := a;
	for i in vbnd..1 by -1 repeat {
	    pi := prev i;
	    (var, at) := I.pi;
	    A.pi := MPmodP!(eval(copy(A.i), var, at), p_^l);
	}

	for j in 1..vbnd repeat {

	    mpow := 1;
	    U1   := copy(U);
	    pj   := prev(j);

	    for i in 0..nbnd repeat {
		worker := uc.i;
		if one? worker then iterate;
		for k in j..prev(vbnd) repeat {
		    (var, at) := I.k; 
		    worker := eval(worker, var, at);
		}
		morker := MPmodP!(worker, p_^l);
		deg  := degree(U.i, mainvar);
		coef := coefficient(U.i, mainvar, deg)*mainmonom^deg;
		U.i  := minus!(U.i, coef);
		U.i  := add!(U.i, times(worker,1,mainvar,deg));
	    }
	    worker := 1;
	    for t in U repeat
		worker := times!(worker, t);
	    e := A.j - worker;
	    (var, at) := I.pj;
	    monom := (var::MPc - at::MPc);

	    for k in 1..(degree(A.j, var)) repeat {
		if zero? e then break;

		mpow := times!(mpow, monom);
		tc   := taylorCoeff(e, var, at, k);
		if ~zero?(tc) then {
		    deltaU := multivariateDiophant(U1,tc,II,mdeg,p,l);

		    if n=2 then {
			delta  := MPmodP!(deltaU.0*deltaU.1, p_^l);
			delta  := MPmodP!(times!(delta,mpow^2::MI), p_^l);
			forget := MPmodP!((deltaU.0*U.1+deltaU.1*U.0), p_^l);
			forget := MPmodP!((forget*mpow+delta), p_^l);
			e := MPmodP!((e-forget), p_^l);
		    }
		    for i in 0..nbnd repeat {
			deltaU.i := MPmodP!(times!(deltaU.i, mpow), p_^l);
			U.i := add!(U.i, deltaU.i);
			U.i := MPmodP!(U.i, p_^l);
		    }
		    free!(deltaU);

		    if n~=2 then {
			worker := 1;
			for t in U repeat
			    worker := times!(worker, t);
			e := A.j - worker;
			e := MPmodP!(e, p_^l);
		    }
		}
	    }
	    II    := resize!(II, j);
	    II.pj := I.pj;
	}
	return U;
    }
}
    
    
-- #if ALDORTEST

--  ----- test mhensel.as -----

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
-- X: Symbol == -"x";
-- Y: Symbol == -"y";
-- Z: Symbol == -"z";

-- symList : List Symbol == [X, Y, Z];


-- makePolyAndFactors(): (POL, Array POL, Array POL) == {
--     import from OVL, POL, INT;

--     x: POL := variable(machine(1))::POL;
--     y: POL := variable(machine(2))::POL;
--     z: POL := variable(machine(3))::POL;

--     q: POL := x^3+2*z*y*x^2-y^2*x+2*z*x*y^4-1;
--     r: POL := x^3+2*z^4*y*x^2-z*y*x+2*z*y^4-1;
--     p: POL := q*r;
--     u: POL := x^3+2*x^2-x+1;
--     v: POL := x^3+2*x^2+x-1;

--     return (p, [u, v], [1, 1]);
-- }

-- p_^k: INT := 0;
-- modPk(c: INT):INT == symmetricMod(c, p_^k);
-- MPmodPk!(f: POL): POL == map!(modPk)(f);

-- mhensel__test(): Boolean == {
--     import from INT, OVL, POL, MI;
--     import from Array POL, Cross(OVL, INT);
--     import from Array Cross(OVL, INT);
--     import from MultivariateHenselLifting(INT, OVL, POL);

--     (a, u, ul) := makePolyAndFactors();
--     ii: Cross(OVL, INT) := (variable(2::MI)$OVL, 1);
--     jj: Cross(OVL, INT) := (variable(3::MI)$OVL, 1);
--     I : Array Cross(OVL, INT) := [ ii, jj ];
--     d: INT := 8;
--     p: INT := 5;
--     l: INT := 2;

--     free p_^k := p^l;

--     res := lift(a, I, d, p, l, u, ul);

--     worker: POL := 1;
--     aa: POL := a;
--     for e in res repeat 
-- 	worker  := worker*e;
--     worker := MPmodPk!(worker);

--     return zero?(worker-a);
-- }

-- stdout << "test mhensel.as .... " << newline;
-- aldorTest("HenselLifting in Z[x,y,z]", mhensel__test);
-- stdout << newline;

-- #endif
