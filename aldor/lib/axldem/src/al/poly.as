#include "axllib.as"

#library PolyCat "polycat.ao"
import from   PolyCat;

macro NonNegativeInteger == Integer;

Term(S: Ring, Expon: AbelianMonoid): BasicType with {
	bracket: (Expon, S) -> %;
	apply: (%, 'coef') -> S;
	apply: (%, 'expon') -> Expon;
}
== add {
	Rep == Record(expon:Expon, coef:S);
	import from Rep;
	sample: % == nil$Pointer pretend %;
	apply(f:%, tag:'coef'):S == rep(f).coef;
	apply(f:%, tag:'expon'):Expon == rep(f).expon;
	[e:Expon, c:S]:% == per [e,c];
	(f1:%) = (f2:%): Boolean == {
		import from S;
		import from Expon;
		rep(f1).expon = rep(f2).expon and rep(f1).coef = rep(f2).coef
	}
	(f1:%) ~= (f2:%): Boolean == {
		import from S;
		import from Expon;
		rep(f1).expon ~= rep(f2).expon or rep(f1).coef ~= rep(f2).coef
	}
	(p:TextWriter) << (f:%):TextWriter == {
		import from S;
		import from Expon;
		rf := rep f;
		rf.expon = 0 => p << rf.coef;
		if rf.coef = 1 then p << "X" else p << rf.coef << "*X";
		p << "^" << rf.expon
	}
}


Polynomial(S: EuclideanDomain,Expon: OrderedAbelianGroup): PolynomialCategory(S,Expon) with {
	var: Expon -> %;
	quo: (%, S) -> %;
--      quo: (%, %) -> %;
	^:   (%, SingleInteger) -> %;
	monicDivide: (%, %) -> Record(quotient:%, remainder:%);
	primitivePart:	% -> %;
	content:	% -> S;
}
== add {
	Rep ==> List Term(S,Expon);
	import from Rep;
	import from 'coef';
	import from 'expon';
	import from S;
	import from Expon;
	import from Term(S,Expon);

	inline from Rep;
	inline from Generator Rep;
	inline from Term(S,Expon);

	0: % == per nil;
	1: % == var(0);
	sample: % == 0;	--!! Shd get from AbelianModoid default

	zero?(x: %): Boolean == empty?(rep x);
	var(n: Expon): % == per cons([n,1], nil);

	degree(x: %): Expon == {
		empty? rep x => 0;
		first(rep x).expon
	}
	leadingCoefficient(x: %): S == {
		empty?(l := rep x) => 0;
		first(l).coef
	}
	reductum(x: %): % == {
		empty?(l := rep x) => x;
		per rest l
	}
	monomial(c: S, e: Expon): % == {
		c = 0 => 0;
		per cons([e,c],nil)
	}
	(xx: %) =  (yy: %): Boolean == rep(xx) = rep(yy);
	(xx: %) ~= (yy: %): Boolean == rep(xx) ~= rep(yy);
--	fixEquals(xx: %, yy: %): Boolean == rep(xx) = rep(yy);
--	fixNotEquals(xx: %, yy: %): Boolean == rep(xx) ~= rep(yy);
	fixEquals(xx: %, yy: %): Boolean == xx = yy;
	fixNotEquals(xx: %, yy: %): Boolean == xx ~= yy;
	+ (x: %): % == x;
	- (x: %): % == per [[t.expon,-t.coef] for t in rep x];
	(xx: %) + (yy: %): % == {
		(x, y) := (rep xx, rep yy);
		not x => yy;
		not y => xx;
		(x0,y0):= (first x,first y);
		y0.expon > x0.expon =>
			per cons(first y,rep(xx + per rest y));
		x0.expon > y0.expon =>
			per cons(first x,rep(per rest x + yy));
		r: S:= x0.coef + y0.coef;
		r = 0 => per rest x + per rest y;
		per cons([x0.expon,r],rep(per rest x + per rest y));
	}
	(xx: %) - (yy: %): % == {
		(x, y) := (rep xx, rep yy);
		not x => - yy;
		not y => xx;
		(x0,y0):= (first x, first y);
		y0.expon > x0.expon =>
			per cons([y0.expon,-y0.coef], rep(xx - per rest y));
		x0.expon > y0.expon =>
			per cons(first x, rep(per rest x - yy));
		r:S:= x0.coef - y0.coef;
		r = 0 => per rest x - per rest y;
		per cons([x0.expon,r], rep(per rest x - per rest y))
	}
	(c: S) * (x: %): % == {
		c = 0 => 0;
		c = 1 => x;
		per [[u.expon,a] for u in rep x | (a := c*u.coef) ~= 0]
	}
	(x:%) * (c:S): % == {
		c = 0 => 0;
		c = 1 => x;
		per [[u.expon,a] for u in rep x | (a := u.coef*c) ~= 0]
	}
	(p1: %) * (p2: %): % == {
		import from SingleInteger;
		l1 := rep p1;
		l2 := rep p2;
		empty? l1 => p1;
		empty? l2 => p2;
		first(l1).expon = 0$Expon => first(l1).coef * p2;
		first(l2).expon = 0$Expon => p1 * first(l2).coef;
		prod: % := 0;
		-- less consing if we multiply terms in reverse order
		if #l1 > #l2 then { (l1,l2) := (l2,l1); (p1,p2) := (p2,p1) }
		l1 := reverse l1;
		for m in l1 repeat prod := prod + m*p2;
		prod
	}
	(m: Term(S,Expon)) * (y: %): % == 
		per [ [m.expon+t2.expon,r]
				for t2 in rep y | (r:S:=m.coef*t2.coef) ~= 0
		];
	monicDivide(p1: %,p2: %): Record(quotient: %, remainder: %) == {
--      	zero? p2 => error "monicDivide: division by 0";
--      	leadingCoefficient p2 ~= 1 => error "Divisor Not Monic";
--      	p2 = 1 => [p1,0];
		zero? p1 => [0,0];
		degree p1 < (n:=degree p2) => [0,p1];
		rout:Rep := nil;
		p2 := per rest rep p2;
		while not(p1 = 0) repeat {
			(u:=degree p1 - n) < 0 => break;
			rout:=cons([u, c:=leadingCoefficient p1], rout);
			p1:=fmecg(per rest rep p1, u, c, p2);
		}
		[per reverse!(rout),p1]
	}

	-- fmegc(p1,e,r,p2) = p1 - r * X**e * p2
	fmecg(p1: %, e: Expon, r: S, p2: %): % == {
		rout: Rep:= nil;
		r  := - r;
		l1 := rep p1;
		for tm in rep p2 repeat {
			e2:= e + tm.expon;
			c2:= r * tm.coef;
			if c2 = 0 then iterate; -- next term
			while l1 and first(l1).expon > e2 repeat {
				rout := cons(first l1,rout);
				l1 := rest l1
			}
			empty? l1 or first(l1).expon < e2 =>
				rout:=cons([e2,c2],rout);
			if (u := first(l1).coef + c2) ~= 0 then
				rout:=cons([e2, u],rout);
			l1 := rest l1;
		}
		per concat!(reverse! rout,l1)
	}

	(x: %) ^ (n: SingleInteger): %  == {
		n < 0  => error "polynomial to negative power";
		n = 0  => 1;
		odd? n => x * (x ^ (n-1));
		(x * x) ^ (n quo 2)
	}

	(x: %) ^ (n: Integer): %  == {
		n < 0  => error "polynomial to negative power";
		n = 0  => 1;
		odd? n => x * (x ^ (n-1));
		(x * x) ^ (n quo 2)
	}

	coerce(s: S): % == per cons([0, s], nil);
	coerce(n: Integer): % == n::S::%;
	coerce(n: SingleInteger): % == n::S::%;

	content(x: %) : S == {
		c: S := 0;
		while fixNotEquals(x, 0) and c ~= 1 repeat {
			c := gcd(c, leadingCoefficient x);
			x := reductum x
		}
		c
	}
	primitivePart(x: %): % == {
		fixEquals(x, 0) => x;
		c := leadingCoefficient x;
		(1 rem c) = 0 => (1@S quo c) * x;
		(c := content x) = 1 => x;
		x quo c
	}
	(x: %) quo (c: S): % == {
		fixEquals(x, 0) => x;
--		x = 0 => x;
		lc := leadingCoefficient x;
		monomial(lc quo c, degree x) + reductum(x) quo c
	}
	(p: TextWriter) << (x: %): TextWriter == {
		xx := rep x;
		not xx => p << 0$S;
		while xx repeat  {
			p << first xx;
			xx := rest xx;
			if xx then p << " + ";
		}
		p
	}
}
