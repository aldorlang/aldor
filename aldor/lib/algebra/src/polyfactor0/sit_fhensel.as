----------------------------  sit_fhensel.as  -------------------------------
#include "algebra"

UnivariateHenselLifting(Z:IntegerCategory, P:UnivariatePolynomialAlgebra0 Z,
	F:PrimeFieldCategory0, PF:UnivariatePolynomialAlgebra0 F): with {
	linearLift:	(P,List PF,Z)		->	Cross(Partial List P,Z);
	linearLift:	(P,F,Z)			->	Cross(Partial Z,Z);
	linearLift:	(P,List F,Z)		->	Cross(Partial List Z,Z);
	upgrade:	PF			->	P;
	downgrade:	P			->	PF;
	upgrade:        F                       ->      Z;
	downgrade:	Z			->	F;
} == add {
	local p:Z		== (characteristic$PF)::Z;
	local p2:Z		== p quo 2;
	local mp:MachineInteger == machine(characteristic$PF)$Integer;

	local reduce:Z -> F == {
		import from PartialFunction(Z, F);
		mapping(specialization(F)$Z);
	}

	local ballift(a:F):Z == {
		x := upgrade a;
		x > p2 => x - p;
		x;
	}

	if F has SmallPrimeFieldCategory0 then {
		upgrade(a:F):Z   == { import from MachineInteger; machine(a)::Z}
		downgrade(a:Z):F == { import from MachineInteger; (a mod mp)::F}
	}
	else {
		upgrade(a:F):Z		== { import from Integer; lift(a)::Z }
		downgrade(a:Z):F	== reduce a;
	}

	upgrade(a:PF):P == {
		import from Z,F;
		r:P := 0;
		for t in a repeat {
			(c, e) := t;
			r := add!(r, ballift c, e);
		}
		r;
	}
	
	downgrade(a:P):PF == {
		import from Z, F;
		r:PF := 0;
		for t in a repeat {
			(c,e) := t;
			 r := add!(r, downgrade c, e);
		}
		r;
	}

	-- returns g (normalized) and s t s.t. a s + b t = g = gcd(a, b);
	local eea(a:PF,b:PF):(PF,PF,PF) == {
		import from F;
		TRACE("fhensel::eea: a = ", a);
		TRACE("fhensel::eea: b = ", b);
		(c, c1, c2) := extendedEuclidean(a, b);
		TRACE("fhensel::eea: c = ", c);
		TRACE("fhensel::eea: c1 = ", c1);
		TRACE("fhensel::eea: c2 = ", c2);
		assert(c1 * a + c2 * b = c);
		ilc := inv leadingCoefficient c;
		g := ilc * c;
		s := ilc * c1;
		t := ilc * c2;
		assert(s * a + t * b = g);
		assert(one? leadingCoefficient g);
		(g,s,t);
	}

	local diophant(a:PF,b:PF,s1:PF,t1:PF,c:PF):(PF,PF) == {
		s := s1*c;
		t := t1*c;
		(q,sigma) := divide(s,b);
		tau := t + q*a;
		(sigma,tau);
	}

	local diophant(a:PF,b:PF,c:PF):(PF,PF) == {
		(g,s1,t1) := eea(a,b);
		assert(g=1);
		diophant(a,b,s1,t1,c);
	}

	linearLift(f:P, up:PF, vp:PF, bound:Z):Cross(Partial Cross(P,P),Z) == {
		import from Boolean, Partial Cross(P,P);
		TRACE("linearLift, f = ", f);
		TRACE("up = ", up);
		TRACE("vp = ", vp);
		TRACE("bound = ", bound);
		lc := leadingCoefficient f;
		f := lc*f;
		lcp := downgrade lc;
		up := lcp * up; vp := lcp * vp;
		u := upgrade up; v := upgrade vp;
		-- at this point, f - u v = 0 mod p
		assert(zero? downgrade(f - u * v));
		-- u := u-monomial(leadingCoefficient u,degree u)+monomial(lc,degree u);
		u := setCoefficient!(u, degree u, lc);
		TRACE("u = ", u);
		-- v := v-monomial(leadingCoefficient v,degree v)+monomial(lc,degree v);
		v := setCoefficient!(v, degree v, lc);
		TRACE("v = ", v);
		e := f-u*v;
		TRACE("e = ", e);
		(g,s1,t1) := eea(vp,up);
		TRACE("gcd(vp,up) = ", g);
		TRACE("coeff_vp = ", s1);
		TRACE("coeff_up = ", t1);
		~one? g => (failed,p);
		assert(one? g);
		i:MachineInteger := 1;
		modulus := p;
		while modulus < bound and (e~=0) repeat {
			import from Partial P;
			TRACE("lifting to p^",i);
#if TRIALDIV
			{ -- trial division
				import from Partial P;
				tq := exactQuotient(f,u);
				~failed? tq => {
					TRACE("trial division succeeded on 1st factor");
					return (u,retract tq);
				}
				tq := exactQuotient(f,v);
				~failed? tq => {
					TRACE("trial division succeeded on 2nd factor");
					return (retract tq,v);
				}
			}
#endif
			c0 := exactQuotient(e,modulus::P);
			failed? c0 => return (failed,modulus);
			c := retract c0;
			if c=0 then {
				du:P := 0; dv:P := 0;
			}
			else {
				TRACE("solving diophantine equation","");
				(dU,dV) := diophant(vp,up,s1,t1,downgrade c);
				du := upgrade dU; dv := upgrade dV;
			}
			TRACE("updating error term","");
			e:= e - modulus*(v*du+u*dv) - (modulus*modulus*(du*dv));
			u := u+modulus*du; v := v+modulus*dv;
			i := next i;
			modulus := modulus * p;
		}
		([(primitivePart u,primitivePart v)],modulus);
	}
	
	linearLift(f:P, lp:List PF, bound:Z):Cross(Partial List P,Z) == {
		import from MachineInteger, Partial List P;
		import from Partial Cross(P,P), List P;
		TRACE("linearLift, f = ", f);
		TRACE("lp = ", lp);
		TRACE("bound = ", bound);
		if #lp=2 then {
			(r,liftk) := linearLift(f,lp(1),lp(2),bound);
			failed? r => return (failed,p);
			(u,v) := retract r;
			return ([[u,v]],liftk);
		}
		up := first lp; lp := rest lp;
		vp:PF := 1;
		for t in lp repeat vp := vp * t;
		(r,k1) := linearLift(f,up,vp,bound);
		liftk := k1;
		TRACE("failed? binary linearLift = ", failed? r);
		failed? r => return (failed,liftk);
		(u,v) := retract r;
		TRACE("u = ", u);
		TRACE("v = ", v);
		(l,k2) := linearLift(v,lp,bound);
		if liftk<k2 then liftk := k2;
		TRACE("failed? recursive linearLift = ", failed? l);
		failed? l => return (failed,liftk);
		([cons(u, retract l)],liftk);
	}

	linearLift(f:P, u:F, bound:Z):Cross(Partial Z,Z) == {
		import from PF,Partial Z;
		TRACE("linearLift, f = ", f);
		TRACE("u = ", u);
		TRACE("bound = ", bound);
		df := differentiate f;
		dfp := downgrade df;
		TRACE("derivative: ",dfp);
		denom := dfp(u);
		zero? denom => (failed, p);
		invden := inv denom;
		assert((downgrade f)(u)=0);
		U := ballift u;
		i:MachineInteger := 1;
		modulus := p;
		while modulus < bound and f(U) ~= 0 repeat {
			TRACE("lifting to p^",i);
			q := exactQuotient(f(U),modulus);
			failed? q => return (failed,modulus);
			qq := downgrade retract q;
			if qq ~= 0 then
				U := U + ballift(-qq * invden) * modulus;
			i := next i;
			modulus := modulus * p;
		}
		([U], modulus);
	}

	linearLift(f:P, ul:List F, bound:Z):Cross(Partial List Z,Z) == {
		import from Partial Z,Partial List Z;
		TRACE("linearLift, f = ", f);
		TRACE("ul = ", ul);
		TRACE("bound = ", bound);
		r:List Z := empty;
		liftk:Z := -1;
		for u in ul repeat {
			(l,k1) := linearLift(f,u,bound);
			if k1>liftk then liftk := k1;
			failed? l => return (failed,liftk);
			r := cons( retract l, r);
		}
		r := reverse! r;
		([r],liftk);
	}
}

