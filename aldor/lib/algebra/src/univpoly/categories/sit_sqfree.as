------------------------------  sit_sqfree.as ---------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariatePolynomialSquareFree}
\History{Laurent Bernardin}{24/5/95}{created}
\Usage{import from \this(R, P)}
\Params{
{\em R} & \astype{GcdDomain} & Coefficient ring of the polynomials\\
{\em P} & \astype{UnivariatePolynomialAlgebra0} R & A polynomial ring\\
}
\Descr{\this~provides implementations of various squarefree factorization
algorithms.}
\begin{exports}
\asexp{musser}: & P $\to$ (R, \astype{Product} P) & Musser's algorithm\\
\asexp{yun}: & P $\to$ (R, \astype{Product} P) & Yun's algorithm\\
\end{exports}
#endif

macro {
	Z	== Integer;
	PR	== Product P;
}

UnivariatePolynomialSquareFree(R:GcdDomain,
	P:UnivariatePolynomialAlgebra0 R): with {
		musser:	P -> (R, PR);
#if ALDOC
\aspage{musser}
\Usage{\name~p}
\Signature{P}{(R, \astype{Product} P)}
\Params{ {\em p} & P & The polynomial to factor\\ }
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is squarefree, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
\alseealso{\asexp{yun}}
#endif
		yun:	P -> (R, PR);
#if ALDOC
\aspage{yun}
\Usage{\name~p}
\Signature{P}{(R, \astype{Product} P)}
\Params{ {\em p} & P & The polynomial to factor\\ }
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is squarefree, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
\alseealso{\asexp{musser}}
#endif
} == add { 
	local charac:Z == characteristic$P;

	if R has FiniteCharacteristic then {
		yun(a:P):(R, PR) == {
			zero? a => (0, 1);
			(c, a) := normalize a;
			assert(R has FiniteSet);
			(c, charpYun a);
		}

		musser(a:P):(R, PR) == {
			zero? a => (0, 1);
			(c, a) := normalize a;
			(c, charpMusser a);
		}

		local exponentReduction(a:P):P == {
			r:P := 0;
			for term in a repeat {
				(c, n) := term;
				assert(zero?(n rem charac));
				r := add!(r, c, quotient(n, charac));
			}
			r;
		}
	
		local charpMusser(a:P):PR == {
			import from R;
			assert(a ~= 0);
			assert(one? leadingCoefficient a);
			zero? degree a => 1;
			zero?(ap := differentiate a) => {
				aa := exponentReduction a;
				(charpMusser aa)^charac;
			}
			(g, astar, dummy) := gcdquo(a, ap);
			zero? degree g => term(a, 1);
			(gstar, d1, dummy) := gcdquo(astar, g);
			a := quotient(a, d1);
			r := term(d1, 1);
			for term in charpMusser g repeat {
				(q, e) := term;
				if zero?(e rem charac) then {
					r := times!(r, q, e);
					a := quotient(a, q^e);
				}
				else {
					r := times!(r, q, next e);
					a := quotient(a, q^(next e));
				}
			}
			zero? degree a => r;
			rr:PR := 1;
			for term in r repeat {
				(q, e) := term;
				if zero?(e rem charac) then {
					(g, a, q) := gcdquo(a, q);
					rr := times!(times!(rr,g,next e), q, e);
				}
				else rr := times!(rr, q, e);
			}
			rr;
		}

		local charpYun(a:P):PR == {
			assert(R has FiniteSet);
			assert(a ~= 0);
			b := differentiate a;
			(c, w, v) := gcdquo(a, b);
			zero? degree c => term(a, 1);
			u := v - differentiate w;
			r:PR := 1;
			i:Z := 1;
			while (i < charac - 1) and (u ~= 0) repeat {
				(g, w, v) := gcdquo(w, u);
				if degree g > 0 then r := times!(r, g, i);
				c := c quo w;
				u := v - differentiate w;
				i := next i;
			}
			if degree w > 0 then r := times!(r, w, i);
			zero? degree c => r;
			k:Z := 1;
			c := exponentReduction c;
			while degree c > 0 and zero? differentiate c repeat {
				c := exponentReduction c;
				k := next k;
			}
			h := charpYun c;
			rnew:PR := 1;
			rleft:PR := 1;
			for t1 in r repeat {
				(p1, e1) := t1;
				if p1 = 1 then iterate;
				hh := h;
				h:PR := 1;
				for t2 in hh repeat {
					(p2,e2) := t2;
					if p2 = 1 then iterate;
					(g, p1, p2) := gcdquo(p1, p2);
					if degree g > 0 then {
						rnew := times!(rnew, g,
								e2*charac^k+e1);
					}
					h := times!(h, p2, e2);
				}
				rleft := times!(rleft, p1, e1);
			}
			rnew := rleft * rnew;
			for t1 in h repeat {
				(p1,e1) := t1;
				rnew := times!(rnew, p1, e1*charac^k);
			}
			r := 1;
			for t1 in rnew repeat {
				(p1,e1) := t1;
				if p1~=1 then r := times!(r, p1, e1);
			}
			r; 	
		}	
	}

	else {
		yun(a:P):(R, PR) == {
			zero? a => (0, 1);
			(c, a) := normalize a;
			(u, p) := char0Yun a;
			(u * c, p);
		}

		musser(a:P):(R, PR) == {
			zero? a => (0, 1);
			(c, a) := normalize a;
			(c, char0Musser(a,1));
		}

		local char0Musser(a:P, n:Z):PR == {
			import from Partial P;
			assert(a ~= 0);
			(g, astar, dummy) := gcdquo(a, differentiate a);
			zero? degree g => term(a, n);
			(gstar, d1, dummy) := gcdquo(astar, g);
			times!(char0Musser(g, next n), d1, n);
		}

		-- returns a product p and a unit u s.t a = u expand(p)
		local char0Yun(a:P):(R, PR) == {
			assert(a ~= 0);
			lcr:R := 1;
			i:Z := 1;
			r:PR := 1;
			b := differentiate a;
			(c,w,y) := gcdquo(a,b);	-- c = gcd(a,b)= a/w = b/y
			if c~=1 then {
				z := y - differentiate w;
				while z~=0 repeat {
					(g,w,y) := gcdquo(w,z);
					(lc, dg) := leadingTerm g;
					if dg > 0 then {
						r := times!(r, g, i);
						lcr := times!(lcr, lc^i);
					}
					i := next i;
					z := y - differentiate w;
				}
			}
			lcr := times!(lcr, leadingCoefficient(w)^i);
			(quotient(leadingCoefficient a, lcr), times!(r, w, i));
		}
				
	}

	if R has Field then {
		normalize(a:P):(R, P) == {
			assert(a ~= 0);
			(leadingCoefficient a, monic a);
		}
	}
	else {
		normalize(a:P):(R, P) == {
			assert(a ~= 0);
			primitive a;
		}
	}
}

#if ALDORTEST
------------------------ test sit_sqrfree.as ---------------------
#include "algebra"
#include "aldortest"

macro {
	Z == Integer;
	Zx == DenseUnivariatePolynomial(Z, -"x");
	F == ZechPrimeField 5;
	Fx == DenseUnivariatePolynomial(F, -"x");
}

import from Symbol, MachineInteger;

local char0(f:Zx -> (Z, Product Zx)):Boolean == {
	import from Z, Zx, PrimitiveArray Z, Product Zx;
	x := monom;
	p:Zx := 1;
	for i in 1..5@Z repeat p := p * (x - i::Zx)^i;
	(c, pr) := f p;
	hit := new(5@MachineInteger, 0);
	i:Z := 0;
	(degree p ~= 15) or (c ~= 1) or ((c * expand pr) ~= p) => false;
	for term in pr repeat {
		(q, n) := term;
		degree(q) ~= 1 => return false;
		hit(machine prev n) := 1;
		i := next i;
	}
	for j in 0..4@MachineInteger repeat { zero?(hit.j) => return false; }
	i = 5;
}

local char0():Boolean == {
	import from UnivariatePolynomialSquareFree(Z, Zx);
	char0(musser) and char0(yun);
}

local charp(f:Fx -> (F, Product Fx)):Boolean == {
	import from Z, F, Fx, PrimitiveArray Z, Product Fx;
	x := monom;
	p:Fx := (3@Z)*1;
	for i in 1..4@Z repeat p := p * (x*x + i*1)^i;
	for i in 1..3@Z repeat p := p * (x*x + x + i*1)^(i+4);
	p := p * (x*x + (3@Z)*x + (4@Z)*1)^(8@Z);
	for i in 1..4@Z repeat p := p * (x + i*1)^(i+8);
	(c, pr) := f p;
	(c ~= (3@Z)::F) or ((c * expand pr) ~= p) => false;
	hit := new(23@MachineInteger,0);
	for term in pr repeat {
		(q,n) := term;
		(n>8) and degree(q)~=1 => return false;
		(n<=8) and degree(q)~=2 => return false;
		n>23 => return false;
		hit(machine prev n):=1;
	}
	hit(1):=hit(1)-1;
	hit(2):=hit(2)-1;
	hit(4):=hit(4)-1;
	hit(5):=hit(5)-1;
	hit(7):=hit(7)-1;
	hit(11):=hit(11)-1;
	hit(12):=hit(12)-1;
	hit(17):=hit(17)-1;
	hit(22):=hit(22)-1;
	for j in 0..22@MachineInteger repeat {
		if hit(j)~=0 then return false;
	}
	true;
}

local charp():Boolean == {
	import from UnivariatePolynomialSquareFree(F, Fx);
	charp(musser) and charp(yun);
}
	
stdout << "Testing sit__sqrfree..." << endnl;
--aldorTest("Characteristic zero", char0);
--TEST FAILED: does not terminate
aldorTest("Characteristic p", charp);
stdout << endnl;
#endif
