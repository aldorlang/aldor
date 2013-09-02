#include "algebra"

macro {
    MI == MachineInteger;
}

UnivariatePolynomialResidueClassRing(R: CommutativeRing,
				     U: UnivariatePolynomialAlgebra0(R),
				     p:U): ResidueClassRing(U,p) == add {
	Rep == PrimitiveArray(R);
	import from R, U, Rep, MI;
	local d__Z: Integer == degree(p);
	local d__MI: MI == machine(d__Z);
	local b: MI == d__MI - 1;

	local m: U == {
	    r?: Partial(R) == reciprocal(leadingCoefficient(p));
	    assert(not failed? r?);
	    r: R := retract(r?);
	    r * p;
	}

	local deg(x: %): MI == {
	    local d: MI := 0;
	    local a: Rep := rep(x);
	    for i in b..0 by -1 repeat {
		not zero?(a.i) => {
		    d := i;
		    break;
		}
	    }
	    d;
	}
	modularRep(u: U): % == {
	    u := monicRemainder(u,m);
	    local a: Rep := new(d__MI, 0$R);
	    for t in u repeat {
		(coef: R, e: Integer) := t;
		a.(machine(e)) := coef;
	    }
	    per a;
	 }
	canonicalPreImage(x: %): U == {
	    local d: MI := deg(x);
	    local a: Rep := rep(x);
	    zero? a.d => 0;
	    local u: U := term(a.d, d :: Integer);
	    for i in 0..(d-1) repeat {
		u := add!(u, a.i, i :: Integer);
	    }
	    u;
	}
	(x: %) = (y: %): Boolean ==  equal?(rep(x), rep(y), d__MI);
	0: % == per new(d__MI, 0$R);
	zero?(x: %): Boolean == {
	    for i in 0..b repeat {not zero?((rep x).i) => return false};
	    true;
	}
	(x: %) + (y: %): % == {
	    local a: Rep := new(d__MI, 0$R);
	    for i in 0..b repeat {
		a.i := (rep x).i + (rep y).i;
	    }
	    per a;
	}
	- (y: %): % == {
	    local a: Rep := new(d__MI, 0$R);
	    for i in 0..b repeat a.i := -  a.i;
	    per a;
	}
	(x: %) - (y: %): % == {
	    local a: Rep := new(d__MI, 0$R);
	    for i in 0..b repeat {
		a.i := (rep x).i - (rep y).i;
	    }
	    per a;
	}
	add!(x: %, y: %): %  == {
	    for i in 0..b repeat (rep x).i := add!((rep x).i, (rep y).i);
	    x;
	}
	minus!(x: %, y: %): %  == {
	    for i in 0..b repeat (rep x).i := minus!((rep x).i, (rep y).i);
	    x;
	}
	minus!(x: %): %  == {
	    for i in 0..b repeat (rep x).i := minus!((rep x).i);
	    x;
	}
	(z: Integer) * (x: %): % == {
	    local a: Rep := new(d__MI, 0$R);
	    for i in 0..b repeat {
		a.i := z * (rep x).i;
	    }
	    per a;
	}
	1: % == {
	    local a: Rep := new(d__MI, 0$R);
	    a.0 := 1;
	    per a;
	}
	one?(x: %): Boolean  == {
	    not one?(rep(x).0) => false;
	    zero?(deg(x));
	}
	characteristic: Integer == characteristic$U;
	coerce(n: Integer): % == {
	    local u: Rep := new(d__MI, 0$R);
	    u.0 := n::R;
	    per u;
	}
	extree(x: %): ExpressionTree == extree(canonicalPreImage(x));

	(x: %) * (y: %): % == {
	    -- IMPROVE LATER IF POSSIBLE .....
	    local u__x: U := canonicalPreImage(x);
	    local u__y: U := canonicalPreImage(y);
	    modularRep(u__x * u__y);
	}
	(x: %) ^ (mi: MI): % == {
	    -- IMPROVE LATER IF POSSIBLE ..
	    import from BinaryPowering(%, MI);
	    binaryExponentiation(x,mi);
	}

	    -- IMPROVE LATER
	-- reciprocal: % -> Partial(%)
	-- canonicalUnitNormal?: Boolean 
	-- reciprocal: % -> Partial(%)
	-- unitNormal: % -> (%, %, %)
	-- unitNormalize: (%, %) -> (%, %)
	-- unit?: % -> Boolean
	

	if U has EuclideanDomain then {
	    symmetricPreImage(x: %): U == canonicalPreImage(x);

	    if U has SourceOfPrimes then {
		local ok: Boolean == {
		    assert(prime?(p));
		    true;
		}
		inv(x: %): % == {
		    -- IMPROVE SOON !!!!!!!!!!!!!!!!!!!!!!!!
		    local u__x: U := canonicalPreImage(x);
		    (g, s, t) := extendedEuclidean(u__x, p);
		    assert(one? g);
		    modularRep(s);
		}
	    }
	}
    }
